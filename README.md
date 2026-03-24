# Sistema de Tickets + Captura Sequencial (Arma Reforger)

Base modular para modo com:

- 5 torres/bandeiras na sequência (configurável);
- Apenas 2 torres ativas no início (configurável);
- Avanço sequencial da linha de frente (libera próxima, bloqueia a que ficou para trás);
- Perda periódica de tickets por time (intervalo configurável);
- Perda de tickets por morte e por respawn (configuráveis);
- Placar de tickets na tela do jogador (HUD).

## Estrutura de arquivos

```text
Addons/SistemaDeTicket/
  Scripts/Game/TicketSystem/
    STK_Config.c
    STK_FlagState.c
    STK_TeamState.c
    STK_SequentialCaptureSystem.c
    STK_TicketSystemComponent.c
    STK_PlayerEventsBridge.c
    STK_TicketHUDComponent.c
  UI/layouts/
    STK_TicketHUD.layout
```

## Configuração principal

`STK_Config.c`:

- `m_iTicketsIniciais = 2000` (cada time começa com 2000 tickets)
- `m_fIntervaloPerdaPeriodicaSeg = 600.0` (10 minutos)
- `m_iPerdaPorMorte = 1`
- `m_iPerdaPorRespawn = 6`

## Eventos usados

- Captura de bandeira: `SCR_CaptureArea.GetOwnershipChangedEvent()`
- Morte do jogador: `SCR_BaseGameMode.GetOnPlayerKilled()`
- Respawn/spawn do jogador: `SCR_BaseGameMode.GetOnPlayerSpawned()`

## Como ligar no World Editor

1. Adicione `STK_TicketSystemComponent` no GameMode (ou entidade central das regras).
2. Registre as 5 áreas de captura no `InitializeServer()` em ordem de avanço.
3. Adicione `STK_TicketHUDComponent` em uma entidade client-side de HUD/UI (ou PlayerController local).
4. Garanta que o layout `Addons/SistemaDeTicket/UI/layouts/STK_TicketHUD.layout` esteja no build do addon.

## Adaptações pendentes (esperadas)

- Mapeamento real de facção -> time em `ResolveTeamByPlayerId(...)` e `TeamIdFromFaction(...)`.
- Lógica concreta de habilitar/desabilitar captura no método `SetFlagCaptureEnabled(...)` de acordo com o prefab da torre.
- Lógica de encerramento de partida em `OnTeamOutOfTickets(...)`.
