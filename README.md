# thinClient_Patcher
A Patcher for thinClient.exe (Nostale GameforgeLauncher) which had the goal to enable multiclient.

Overwrites the target byte to JMP (0xEB) to jump to the start client code line each time on function call.
