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

### 1) Mapeamento real de facção -> time

Agora o `STK_TicketSystemComponent` já está preparado para o mod externo de facções
**Ucrânia x Rússia** usando `FactionKey`:

- Ucrânia -> `TEAM_A` (`UKR` / `UKRAINE`)
- Rússia -> `TEAM_B` (`RUS` / `RUSSIA`)

Fluxo:
1. Recebe `playerId` (morte/respawn) ou `Faction` (captura).
2. Lê `FactionKey`.
3. Compara com as chaves acima e retorna `TEAM_A` ou `TEAM_B`.

Se o seu mod externo usar chaves diferentes, altere apenas o método
`ResolveTeamByFaction(...)` em `STK_TicketSystemComponent`.

### 2) Habilitar/desabilitar captura da torre

No fluxo atual, a captura já é bloqueada de forma confiável pela regra de negócio:

- Cada torre tem `m_bAtivaParaCaptura`.
- Se uma torre bloqueada disparar evento, o sistema ignora no `OnOwnershipChanged(...)`.

Para o bloqueio ficar também *visual* (ícone/luz/interação), implemente no método
`SetFlagCaptureEnabled(...)` de `STK_SequentialCaptureSystem` de acordo com o prefab da sua torre
(por exemplo, desativar trigger de captura, trocar material/luz, desativar interação).

### 3) Encerramento da partida ao zerar tickets

`OnTeamOutOfTickets(...)` agora já:

- marca `m_bMatchFinished = true`;
- para a contagem periódica (`m_bStarted = false`);
- calcula e loga o time vencedor.

Você só precisa conectar essa parte ao seu fluxo final de modo de jogo (HUD de vitória, tela de fim e reinício da rodada).
