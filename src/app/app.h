#ifndef CSPRITE_SRC_APP_APP_H_INCLUDED_
#define CSPRITE_SRC_APP_APP_H_INCLUDED_
#pragma once

int AppInit(void);
int AppMainLoop(void);
void AppDestroy(void);

void AppOpenURL(const char* url);

#endif
