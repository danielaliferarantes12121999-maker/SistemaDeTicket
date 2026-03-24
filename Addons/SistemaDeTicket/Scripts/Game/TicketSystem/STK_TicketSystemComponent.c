//------------------------------------------------------------------------------------------------
// Componente principal de tickets + integração com captura sequencial.
// Anexe ao GameMode (ou entidade central de regras) no World Editor.
//------------------------------------------------------------------------------------------------
[ComponentEditorProps(category: "GameScripted/GameMode", description: "Sistema de tickets e captura sequencial")]
class STK_TicketSystemComponentClass : ScriptComponentClass {}

// Callback para UI local: tickets time A e tickets time B
void STK_OnTicketScoreChanged(int ticketsTeamA, int ticketsTeamB);
typedef func STK_OnTicketScoreChanged;

class STK_TicketSystemComponent : ScriptComponent
{
	protected ref STK_Config m_pConfig;
	protected ref STK_SequentialCaptureSystem m_pCaptureSystem;
	protected ref map<int, ref STK_TeamState> m_mTeams;
	protected ref STK_PlayerEventsBridge m_pPlayerEventsBridge;

	protected float m_fAcumuladorPeriodico = 0.0;
	protected bool m_bStarted = false;

	// IDs de time definidos pelo seu mod (exemplo 0 e 1)
	protected const int TEAM_A = 0;
	protected const int TEAM_B = 1;

	// Replicação de placar para clientes (HUD)
	[RplProp(onRplName: "OnReplicatedScoreChanged")]
	protected int m_iRepTicketsTeamA;

	[RplProp(onRplName: "OnReplicatedScoreChanged")]
	protected int m_iRepTicketsTeamB;

	protected ref ScriptInvokerBase<STK_OnTicketScoreChanged> m_OnTicketScoreChanged;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);

		m_OnTicketScoreChanged = new ScriptInvokerBase<STK_OnTicketScoreChanged>();

		if (Replication.IsServer())
			InitializeServer();
	}

	override void EOnDelete(IEntity owner)
	{
		if (Replication.IsServer() && m_pPlayerEventsBridge)
			m_pPlayerEventsBridge.Unbind();

		super.EOnDelete(owner);
	}

	protected void InitializeServer()
	{
		m_pConfig = new STK_Config();

		m_mTeams = new map<int, ref STK_TeamState>();
		m_mTeams.Set(TEAM_A, new STK_TeamState(TEAM_A, m_pConfig.m_iTicketsIniciais));
		m_mTeams.Set(TEAM_B, new STK_TeamState(TEAM_B, m_pConfig.m_iTicketsIniciais));

		m_pCaptureSystem = new STK_SequentialCaptureSystem(m_pConfig);
		m_pCaptureSystem.m_OnFlagCaptured.Insert(OnFlagCaptured);

		// TODO: registrar aqui, em ordem de avanço, as 5 áreas de captura do mapa.
		// Exemplo:
		// m_pCaptureSystem.RegisterFlag(m_FlagA_CaptureArea);
		// m_pCaptureSystem.RegisterFlag(m_FlagB_CaptureArea);
		// ...

		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		m_pPlayerEventsBridge = new STK_PlayerEventsBridge(gameMode, this);
		m_pPlayerEventsBridge.Bind();

		m_pCaptureSystem.Start();
		m_bStarted = true;

		SyncReplicatedScore();
	}

	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (!Replication.IsServer() || !m_bStarted)
			return;

		m_fAcumuladorPeriodico += timeSlice;
		if (m_fAcumuladorPeriodico < m_pConfig.m_fIntervaloPerdaPeriodicaSeg)
			return;

		m_fAcumuladorPeriodico = 0.0;
		ApplyPeriodicTicketLoss();
	}

	//--------------------------------------------------------------------------------------------
	// PERDA PERIÓDICA: a cada intervalo, perde (base * nº de bandeiras controladas)
	//--------------------------------------------------------------------------------------------
	protected void ApplyPeriodicTicketLoss()
	{
		ApplyPeriodicLossForTeam(TEAM_A);
		ApplyPeriodicLossForTeam(TEAM_B);
	}

	protected void ApplyPeriodicLossForTeam(int teamId)
	{
		STK_TeamState teamState = m_mTeams.Get(teamId);
		if (!teamState)
			return;

		int flags = m_pCaptureSystem.CountFlagsControlledByTeam(teamId);
		int loss = m_pConfig.m_iPerdaBasePeriodica * flags;
		SubtractTickets(teamId, loss, string.Format("Perda periódica (%1 flags)", flags));
	}

	//--------------------------------------------------------------------------------------------
	// EVENTOS DE JOGO (morte / respawn)
	//--------------------------------------------------------------------------------------------
	void OnPlayerKilledServer(int victimPlayerId)
	{
		int teamId = ResolveTeamByPlayerId(victimPlayerId);
		if (teamId < 0)
			return;

		SubtractTickets(teamId, m_pConfig.m_iPerdaPorMorte, "Perda por morte");
	}

	void OnPlayerRespawnServer(int playerId)
	{
		int teamId = ResolveTeamByPlayerId(playerId);
		if (teamId < 0)
			return;

		SubtractTickets(teamId, m_pConfig.m_iPerdaPorRespawn, "Perda por respawn");
	}

	//--------------------------------------------------------------------------------------------
	// CAPTURA DE BANDEIRA
	//--------------------------------------------------------------------------------------------
	protected void OnFlagCaptured(int flagIdx, int previousOwnerTeam, int newOwnerTeam)
	{
		if (previousOwnerTeam == newOwnerTeam)
			return;

		// Gancho para HUD/áudio/objetivos
		Print(string.Format("[TicketSystem] Flag %1 capturada: %2 -> %3", flagIdx, previousOwnerTeam, newOwnerTeam));
	}

	//--------------------------------------------------------------------------------------------
	// UTILITÁRIOS
	//--------------------------------------------------------------------------------------------
	protected void SubtractTickets(int teamId, int amount, string reason)
	{
		if (amount <= 0)
			return;

		STK_TeamState teamState = m_mTeams.Get(teamId);
		if (!teamState)
			return;

		teamState.m_iTickets = Math.Max(0, teamState.m_iTickets - amount);
		Print(string.Format("[TicketSystem] Team=%1 -%2 (%3) => %4", teamId, amount, reason, teamState.m_iTickets));

		SyncReplicatedScore();

		if (teamState.m_iTickets <= 0)
			OnTeamOutOfTickets(teamId);
	}

	protected void SyncReplicatedScore()
	{
		m_iRepTicketsTeamA = GetTickets(TEAM_A);
		m_iRepTicketsTeamB = GetTickets(TEAM_B);
		Replication.BumpMe();

		// Executa também no servidor para debug local/headless
		OnReplicatedScoreChanged();
	}

	protected void OnReplicatedScoreChanged()
	{
		if (!m_OnTicketScoreChanged)
			return;

		m_OnTicketScoreChanged.Invoke(m_iRepTicketsTeamA, m_iRepTicketsTeamB);
	}

	protected void OnTeamOutOfTickets(int teamId)
	{
		// TODO: finalizar partida e declarar vencedor
		Print(string.Format("[TicketSystem] Team %1 sem tickets.", teamId));
	}

	protected int ResolveTeamByPlayerId(int playerId)
	{
		// Adaptação necessária: recuperar facção/time real via PlayerManager + FactionAffiliation.
		// Retorno mock para facilitar início.
		return (playerId % 2 == 0) ? TEAM_A : TEAM_B;
	}

	int GetTickets(int teamId)
	{
		STK_TeamState teamState = m_mTeams.Get(teamId);
		if (!teamState)
			return 0;

		return teamState.m_iTickets;
	}

	ScriptInvokerBase<STK_OnTicketScoreChanged> GetOnTicketScoreChanged()
	{
		return m_OnTicketScoreChanged;
	}

	string GetFormattedScoreboardText()
	{
		return string.Format("TIME A: %1  |  TIME B: %2", m_iRepTicketsTeamA, m_iRepTicketsTeamB);
	}
};
