#ifndef STRINGS_H
#define STRINGS_H

#ifndef GC
#define STRING_CANCEL "Press +/Y to cancel"
#define STRING_CONTROLS "Press 2/B for controls."
#define STRING_CONTROLS_BOX \
"╔════════════════════════════╦══════════════════════╦═══════════════════════╗" \
"║ -/X      - Change BG color ║ +/Y - Change flavor  ║              \e[4mControls\e[0m\e[104;37m ║" \
"║ A        - Toggle music    ║                      ║                       ║" \
"║ 1/Z      - Send GBA Donut  ║                      ║                       ║" \
"║ \xfd\xfe/START - Exit            ║                      ║ Press 2/B to go back. ║" \
"╚════════════════════════════╩══════════════════════╩═══════════════════════╝"
#else
#define STRING_CANCEL "  Press Y to cancel"
#define STRING_CONTROLS "  Press B for controls."
#define STRING_CONTROLS_BOX \
"╔════════════════════════════╦══════════════════════╦═══════════════════════╗" \
"║ X        - Change BG color ║ Y - Change flavor    ║              \e[4mControls\e[0m\e[104;37m ║" \
"║ A        - Toggle music    ║                      ║                       ║" \
"║ Z        - Send GBA Donut  ║                      ║                       ║" \
"║ START    - Exit            ║                      ║   Press B to go back. ║" \
"╚════════════════════════════╩══════════════════════╩═══════════════════════╝"
#endif

#endif
