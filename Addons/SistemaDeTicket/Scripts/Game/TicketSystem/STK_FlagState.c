//------------------------------------------------------------------------------------------------
// Representa uma torre/bandeira na sequência de avanço.
// Cada posição deve referenciar uma área de captura do mapa.
//------------------------------------------------------------------------------------------------
class STK_FlagState
{
	int m_iIndice;
	SCR_CaptureArea m_pCaptureArea;

	// Dono atual da bandeira, -1 = neutra
	int m_iOwningTeamId = -1;

	// Controla se a captura está habilitada para esta posição da sequência
	bool m_bAtivaParaCaptura = false;

	void STK_FlagState(int indice, SCR_CaptureArea area)
	{
		m_iIndice = indice;
		m_pCaptureArea = area;
	}
};
