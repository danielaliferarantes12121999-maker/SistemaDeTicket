void STK_OnFlagCaptured(int flagIndex, int previousOwnerTeam, int newOwnerTeam);
typedef func STK_OnFlagCaptured;

//------------------------------------------------------------------------------------------------
// Regras de captura sequencial das torres.
// Fluxo:
// 1) Início com N torres ativas (padrão 2)
// 2) Se uma torre ativa é capturada por um time, avança a janela de torres ativas
// 3) Torre "para trás" é bloqueada novamente
//------------------------------------------------------------------------------------------------
class STK_SequentialCaptureSystem
{
	protected ref STK_Config m_pConfig;
	protected ref array<ref STK_FlagState> m_aFlags = {};

	// Índice inicial da "janela ativa" dentro da sequência
	protected int m_iJanelaInicio = 0;

	// Callback para notificar o sistema principal de que uma bandeira mudou
	ref ScriptInvokerBase<STK_OnFlagCaptured> m_OnFlagCaptured;
	// parâmetros: (flagIndex, previousOwnerTeam, newOwnerTeam)

	void STK_SequentialCaptureSystem(STK_Config config)
	{
		m_pConfig = config;
		m_OnFlagCaptured = new ScriptInvokerBase<STK_OnFlagCaptured>();
	}

	// Registre as 5 torres (ou quantidade configurada) em ordem de avanço.
	void RegisterFlag(SCR_CaptureArea captureArea)
	{
		int idx = m_aFlags.Count();
		m_aFlags.Insert(new STK_FlagState(idx, captureArea));
	}

	void Start()
	{
		ActivateInitialWindow();
		BindCaptureEvents();
	}

	protected void ActivateInitialWindow()
	{
		int total = m_aFlags.Count();
		int janela = Math.Clamp(m_pConfig.m_iTorresAtivasIniciais, 1, total);

		for (int i = 0; i < total; i++)
		{
			bool ativa = (i >= 0 && i < janela);
			SetFlagCaptureEnabled(i, ativa);
		}
	}

	protected void BindCaptureEvents()
	{
		foreach (STK_FlagState flagState : m_aFlags)
		{
			if (!flagState.m_pCaptureArea)
				continue;

			// Evento oficial da área de captura.
			// Assinatura esperada: (Faction previousFaction, Faction newFaction)
			flagState.m_pCaptureArea.GetOwnershipChangedEvent().Insert(OnOwnershipChanged);
		}
	}

	protected void OnOwnershipChanged(Faction previousFaction, Faction newFaction)
	{
		int flagIdx = FindFlagIndexByOwnerFactionEvent(previousFaction, newFaction);
		if (flagIdx < 0)
			return;

		STK_FlagState flagState = m_aFlags[flagIdx];
		if (!flagState.m_bAtivaParaCaptura)
			return; // ignora capturas em bandeira bloqueada

		int prevTeam = flagState.m_iOwningTeamId;
		int newTeam = TeamIdFromFaction(newFaction);
		flagState.m_iOwningTeamId = newTeam;

		if (m_OnFlagCaptured)
			m_OnFlagCaptured.Invoke(flagIdx, prevTeam, newTeam);

		AdvanceWindowAfterCapture(flagIdx);
	}

	// Avança a janela de captura: bloqueia bandeira "mais antiga" e libera próxima da sequência.
	protected void AdvanceWindowAfterCapture(int capturedIdx)
	{
		int tamanhoJanela = Math.Clamp(m_pConfig.m_iTorresAtivasIniciais, 1, m_aFlags.Count());
		int novoInicio = Math.Max(m_iJanelaInicio, capturedIdx - (tamanhoJanela - 1) + 1);

		if (novoInicio == m_iJanelaInicio)
			return;

		m_iJanelaInicio = novoInicio;
		RefreshActiveWindow();
	}

	protected void RefreshActiveWindow()
	{
		int total = m_aFlags.Count();
		int tamanhoJanela = Math.Clamp(m_pConfig.m_iTorresAtivasIniciais, 1, total);
		int fim = m_iJanelaInicio + tamanhoJanela - 1;

		for (int i = 0; i < total; i++)
		{
			bool ativa = (i >= m_iJanelaInicio && i <= fim);
			SetFlagCaptureEnabled(i, ativa);
		}
	}

	protected void SetFlagCaptureEnabled(int flagIndex, bool enable)
	{
		if (flagIndex < 0 || flagIndex >= m_aFlags.Count())
			return;

		STK_FlagState flag = m_aFlags[flagIndex];
		flag.m_bAtivaParaCaptura = enable;

		// Adaptação de mapa/editor:
		// aqui você deve habilitar/desabilitar a entidade de captura visual/funcional.
		// Dependendo do prefab usado, pode ser via componente custom, trigger, action ou script próprio.
		// Exemplo: flag.m_pCaptureArea.EnablePeriodicQueries(enable);
	}

	protected int TeamIdFromFaction(Faction faction)
	{
		if (!faction)
			return -1;

		// Adapte ao seu mapeamento real de facções.
		// Exemplo: US=0 / USSR=1
		return faction.GetFactionKey().Hash() % 2;
	}

	protected int FindFlagIndexByOwnerFactionEvent(Faction previousFaction, Faction newFaction)
	{
		// Como o callback de ownership não traz referência explícita da área em algumas integrações,
		// você pode substituir este método por handlers por-bandeira (closure/proxy component).
		// Este fallback procura por mudança no owner atual da área.
		for (int i = 0; i < m_aFlags.Count(); i++)
		{
			STK_FlagState fs = m_aFlags[i];
			if (!fs.m_pCaptureArea)
				continue;

			Faction current = fs.m_pCaptureArea.GetOwningFaction();
			if (current == newFaction)
				return i;
		}

		return -1;
	}

	int CountFlagsControlledByTeam(int teamId)
	{
		int count = 0;
		foreach (STK_FlagState fs : m_aFlags)
		{
			if (fs.m_iOwningTeamId == teamId)
				count++;
		}

		return count;
	}
};
