//------------------------------------------------------------------------------------------------
// Bridge responsável por inscrever nos eventos do GameMode e repassar ao sistema de tickets.
//------------------------------------------------------------------------------------------------
class STK_PlayerEventsBridge
{
	protected SCR_BaseGameMode m_pGameMode;
	protected STK_TicketSystemComponent m_pTicketSystem;

	void STK_PlayerEventsBridge(SCR_BaseGameMode gameMode, STK_TicketSystemComponent ticketSystem)
	{
		m_pGameMode = gameMode;
		m_pTicketSystem = ticketSystem;
	}

	void Bind()
	{
		if (!m_pGameMode || !m_pTicketSystem)
			return;

		// Morte: invoker padrão do SCR_BaseGameMode
		m_pGameMode.GetOnPlayerKilled().Insert(OnPlayerKilled);

		// Respawn/Spawn: em Reforger, GetOnPlayerSpawned cobre respawns e spawn inicial.
		m_pGameMode.GetOnPlayerSpawned().Insert(OnPlayerSpawned);
	}

	void Unbind()
	{
		if (!m_pGameMode)
			return;

		m_pGameMode.GetOnPlayerKilled().Remove(OnPlayerKilled);
		m_pGameMode.GetOnPlayerSpawned().Remove(OnPlayerSpawned);
	}

	protected void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		m_pTicketSystem.OnPlayerKilledServer(playerId);
	}

	protected void OnPlayerSpawned(int playerId, IEntity playerEntity)
	{
		m_pTicketSystem.OnPlayerRespawnServer(playerId);
	}
};
