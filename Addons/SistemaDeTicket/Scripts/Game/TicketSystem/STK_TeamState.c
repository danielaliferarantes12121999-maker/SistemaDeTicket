//------------------------------------------------------------------------------------------------
// Estado runtime por time.
//------------------------------------------------------------------------------------------------
class STK_TeamState
{
	int m_iTeamId;
	int m_iTickets;
	int m_iFlagsControladas;

	void STK_TeamState(int teamId, int ticketsIniciais)
	{
		m_iTeamId = teamId;
		m_iTickets = ticketsIniciais;
		m_iFlagsControladas = 0;
	}
};
