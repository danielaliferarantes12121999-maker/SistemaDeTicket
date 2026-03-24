//------------------------------------------------------------------------------------------------
// HUD local do jogador para mostrar o placar de tickets na tela.
//------------------------------------------------------------------------------------------------
[ComponentEditorProps(category: "GameScripted/HUD", description: "HUD simples de tickets")]
class STK_TicketHUDComponentClass : ScriptComponentClass {}

class STK_TicketHUDComponent : ScriptComponent
{
	// Caminho de layout do widget do placar.
	// IMPORTANTE: ajuste para o caminho final do seu addon/prefab.
	[Attribute("Addons/SistemaDeTicket/UI/layouts/STK_TicketHUD.layout", UIWidgets.EditBox, "Layout com um TextWidget chamado ScoreText")]
	protected ResourceName m_sHudLayout;

	protected Widget m_wRoot;
	protected TextWidget m_wScoreText;
	protected STK_TicketSystemComponent m_pTicketSystem;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);

		if (Replication.IsServer())
			return; // HUD só no cliente

		SetupHud();
		BindTicketSystem();
	}

	override void EOnDelete(IEntity owner)
	{
		if (m_pTicketSystem)
			m_pTicketSystem.GetOnTicketScoreChanged().Remove(OnScoreChanged);

		if (m_wRoot)
			m_wRoot.RemoveFromHierarchy();

		super.EOnDelete(owner);
	}

	protected void SetupHud()
	{
		WorkspaceWidget workspace = GetGame().GetWorkspace();
		if (!workspace)
			return;

		if (!m_sHudLayout || m_sHudLayout == string.Empty)
			return;

		m_wRoot = workspace.CreateWidgets(m_sHudLayout);
		if (!m_wRoot)
			return;

		m_wScoreText = TextWidget.Cast(m_wRoot.FindAnyWidget("ScoreText"));
		if (m_wScoreText)
			m_wScoreText.SetText("TIME A: 2000  |  TIME B: 2000");
	}

	protected void BindTicketSystem()
	{
		SCR_BaseGameMode gm = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!gm)
			return;

		m_pTicketSystem = STK_TicketSystemComponent.Cast(gm.FindComponent(STK_TicketSystemComponent));
		if (!m_pTicketSystem)
			return;

		m_pTicketSystem.GetOnTicketScoreChanged().Insert(OnScoreChanged);
		OnScoreChanged(2000, 2000); // valor inicial local até chegar replicação real
	}

	protected void OnScoreChanged(int ticketsTeamA, int ticketsTeamB)
	{
		if (!m_wScoreText)
			return;

		m_wScoreText.SetText(string.Format("TIME A: %1  |  TIME B: %2", ticketsTeamA, ticketsTeamB));
	}
};
