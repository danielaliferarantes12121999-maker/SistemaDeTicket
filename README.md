# Sistema de Tickets + Captura Sequencial (Arma Reforger)

Este repositório contém uma base **modular** para um modo com:

- 5 torres/bandeiras na sequência (configurável);
- Apenas 2 torres ativas no início (configurável);
- Avanço sequencial da linha de frente (libera próxima, bloqueia a que ficou para trás);
- Perda periódica de tickets por time (intervalo configurável);
- Perda de tickets por morte e por respawn (configuráveis).

## Estrutura de arquivos

```text
Addons/SistemaDeTicket/Scripts/Game/TicketSystem/
  STK_Config.c
  STK_FlagState.c
  STK_TeamState.c
  STK_SequentialCaptureSystem.c
  STK_TicketSystemComponent.c
  STK_PlayerEventsBridge.c
```

## O que cada arquivo faz

- `STK_Config.c`
  - Centraliza parâmetros de balanceamento (sem precisar mexer na lógica).

- `STK_FlagState.c`
  - Estado de cada torre (índice, área de captura, dono atual, ativa/bloqueada).

- `STK_TeamState.c`
  - Estado de tickets por time.

- `STK_SequentialCaptureSystem.c`
  - Regra de janela de captura sequencial.
  - Inscrição no evento de captura da `SCR_CaptureArea`.

- `STK_TicketSystemComponent.c`
  - Orquestra tudo no servidor:
    - inicializa times;
    - integra captura sequencial;
    - aplica perda periódica;
    - aplica perda por morte/respawn;
    - valida fim de tickets.

- `STK_PlayerEventsBridge.c`
  - Faz o bind dos eventos do `SCR_BaseGameMode` para repassar ao componente de tickets.

## Onde colocar no mod

1. Copie os scripts para:
   - `Addons/SistemaDeTicket/Scripts/Game/TicketSystem/`

2. No **World Editor**:
   - Adicione `STK_TicketSystemComponent` na entidade de Game Mode (ou entidade central de regras).
   - Garanta que seu Game Mode herde/tenha acesso a `SCR_BaseGameMode` para os invokers de player.

3. No código de inicialização (`STK_TicketSystemComponent.Initialize()`):
   - Registre as 5 áreas de captura na ordem da sequência com `RegisterFlag(...)`.

## Eventos principais usados (Reforger)

- Captura de bandeira:
  - `SCR_CaptureArea.GetOwnershipChangedEvent()`

- Morte do jogador:
  - `SCR_BaseGameMode.GetOnPlayerKilled()`

- Respawn do jogador:
  - `SCR_BaseGameMode.GetOnPlayerSpawned()`
  - Observação: este evento normalmente cobre spawn inicial e respawn; se quiser cobrar só respawn, adicione filtro por estado do jogador.

## Pontos que exigem adaptação no seu mapa/projeto

- **Ativar/Bloquear captura visual/funcional**
  - No método `SetFlagCaptureEnabled(...)`, ligar/desligar a mecânica da torre conforme o prefab usado.

- **Mapeamento de facções para teamId**
  - Em `TeamIdFromFaction(...)` e `ResolveTeamByPlayerId(...)`, adapte para sua lógica real (US x USSR, BLUFOR x OPFOR, etc).

- **Fim de partida quando tickets acabam**
  - Implementar dentro de `OnTeamOutOfTickets(...)` integrando com seu fluxo de match-end.

## Parâmetros fáceis de alterar

No `STK_Config.c` você altera rapidamente:

- quantidade de bandeiras (`m_iTotalTorres`);
- intervalo de perda periódica (`m_fIntervaloPerdaPeriodicaSeg`);
- perda por morte (`m_iPerdaPorMorte`);
- perda por respawn (`m_iPerdaPorRespawn`).

---

Se quiser, o próximo passo é eu te entregar uma **segunda versão** com:

- componente editor-friendly para arrastar/soltar as 5 torres direto nos atributos;
- RPC + HUD básico de tickets;
- bloqueio visual explícito por torre (luz, ícone e interação);
- validações de debug no servidor (logs + comandos admin).
