//------------------------------------------------------------------------------------------------
// Configuração central do sistema. Ajuste aqui para balanceamento.
//------------------------------------------------------------------------------------------------
class STK_Config
{
	// Quantidade total de torres no mapa (modular)
	int m_iTotalTorres = 5;

	// Quantas torres começam ativas para captura (neste modo: 2)
	int m_iTorresAtivasIniciais = 2;

	// Intervalo da perda periódica de tickets em segundos (10 minutos = 600)
	float m_fIntervaloPerdaPeriodicaSeg = 600.0;

	// Perda base de tickets por intervalo; será multiplicada por torres capturadas
	int m_iPerdaBasePeriodica = 1;

	// Perda fixa por morte
	int m_iPerdaPorMorte = 1;

	// Perda fixa por respawn
	int m_iPerdaPorRespawn = 6;

	// Tickets iniciais de cada time
	int m_iTicketsIniciais = 2000;
};
