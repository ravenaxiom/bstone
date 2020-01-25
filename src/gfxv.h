/*
BStone: A Source port of
Blake Stone: Aliens of Gold and Blake Stone: Planet Strike

Copyright (c) 1992-2013 Apogee Entertainment, LLC
Copyright (c) 2013-2020 Boris I. Bendovsky (bibendovsky@hotmail.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the
Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/


#ifndef BSTONE_GFXV_INCLUDED
#define BSTONE_GFXV_INCLUDED


#include <cstdint>


extern std::int16_t TELEPORT_LUMP_START;
extern std::int16_t TELEPORT_LUMP_END;

extern std::int16_t README_LUMP_START;
extern std::int16_t README_LUMP_END;

extern std::int16_t CONTROLS_LUMP_START;
extern std::int16_t CONTROLS_LUMP_END;

extern std::int16_t LATCHPICS_LUMP_START;
extern std::int16_t LATCHPICS_LUMP_END;


//
// Amount of each data item
//
extern std::int16_t NUMCHUNKS;
extern std::int16_t NUMFONT;
extern std::int16_t NUMFONTM;
extern std::int16_t NUMPICS;
extern std::int16_t NUMPICM;
extern std::int16_t NUMSPRITES;
extern std::int16_t NUMTILE8;
extern std::int16_t NUMTILE8M;
extern std::int16_t NUMTILE16;
extern std::int16_t NUMTILE16M;
extern std::int16_t NUMTILE32;
extern std::int16_t NUMTILE32M;
extern std::int16_t NUMEXTERNS;

//
// File offsets for data items
//
extern std::int16_t STRUCTPIC;

extern std::int16_t STARTFONT;
extern std::int16_t STARTFONTM;
extern std::int16_t STARTPICS;
extern std::int16_t STARTPICM;
extern std::int16_t STARTSPRITES;
extern std::int16_t STARTTILE8;
extern std::int16_t STARTTILE8M;
extern std::int16_t STARTTILE16;
extern std::int16_t STARTTILE16M;
extern std::int16_t STARTTILE32;
extern std::int16_t STARTTILE32M;
extern std::int16_t STARTEXTERNS;


/* FIXME Unknown AOG pictures
TELEPORT11ONPIC,                     // 17
TELEPORT12ONPIC,                     // 18
TELEPORT13ONPIC,                     // 19
TELEPORT14ONPIC,                     // 20
TELEPORT15ONPIC,                     // 21
TELEPORT16ONPIC,                     // 22
TELEPORT17ONPIC,                     // 23
TELEPORT18ONPIC,                     // 24
TELEPORT19ONPIC,                     // 25
TELEPORT20ONPIC,                     // 26
TELEUPONPIC,                         // 27
TELEDNONPIC,                         // 28
TELEUPOFFPIC,                        // 29
TELEDNOFFPIC,                        // 30
TELEPORT11OFFPIC,                    // 41
TELEPORT12OFFPIC,                    // 42
TELEPORT13OFFPIC,                    // 43
TELEPORT14OFFPIC,                    // 44
TELEPORT15OFFPIC,                    // 45
TELEPORT16OFFPIC,                    // 46
TELEPORT17OFFPIC,                    // 47
TELEPORT18OFFPIC,                    // 48
TELEPORT19OFFPIC,                    // 49
TELEPORT20OFFPIC,                    // 50
AUTOMAP_MAG1PIC,                     // 57
AUTOMAP_MAG2PIC,                     // 58
AUTOMAP_MAG4PIC,                     // 59
H_6PIC,                              // 97
H_TILDEPIC,                          // 98
STARPORTPIC,                         // 133
BOSSPIC,                             // 134
THREEPLANETSPIC,                     // 135
SOLARSYSTEMPIC,                      // 136
AOGENDINGPIC,                        // 137
GFLOGOSPIC,                          // 138
BLAKEHEADPIC,                        // 139
PROJECTFOLDERPIC,                    // 140
WEAPON6PIC,                          // 147
WEAPON7PIC,                          // 148
W1_CORNERPIC,                        // 149
W2_CORNERPIC,                        // 150
W3_CORNERPIC,                        // 151
W4_CORNERPIC,                        // 152
W5_CORNERPIC,                        // 153
W6_CORNERPIC,                        // 154
NG_BLANKPIC,                         // 172
NG_0PIC,                             // 173
NG_1PIC,                             // 174
NG_2PIC,                             // 175
NG_3PIC,                             // 176
NG_4PIC,                             // 177
NG_5PIC,                             // 178
NG_6PIC,                             // 179
NG_7PIC,                             // 180
NG_8PIC,                             // 181
NG_9PIC,                             // 182
NO_KEYPIC,                           // 191
RED_KEYPIC,                          // 192
YEL_KEYPIC,                          // 193
BLU_KEYPIC,                          // 194
ENDINGPALETTE,                       // 199
NO386SCREEN,                         // 203
T_DEMO0,                             // 207
T_DEMO1,                             // 208
T_DEMO2,                             // 209
T_DEMO3,                             // 210
T_DEMO4,                             // 211
T_DEMO5,                             // 212
DECOY,                               // 229
DECOY2,                              // 230
DECOY3,                              // 231
DECOY4,                              // 232
*/


extern std::int16_t TELEPORTBACKPIC;
extern std::int16_t TELEPORT1OFFPIC;
extern std::int16_t TELEPORT2OFFPIC;
extern std::int16_t TELEPORT3OFFPIC;
extern std::int16_t TELEPORT4OFFPIC;
extern std::int16_t TELEPORT5OFFPIC;
extern std::int16_t TELEPORT6OFFPIC;
extern std::int16_t TELEPORT7OFFPIC;
extern std::int16_t TELEPORT8OFFPIC;
extern std::int16_t TELEPORT9OFFPIC;
extern std::int16_t TELEPORT10OFFPIC;
extern std::int16_t TELEPORT1ONPIC;
extern std::int16_t TELEPORT2ONPIC;
extern std::int16_t TELEPORT3ONPIC;
extern std::int16_t TELEPORT4ONPIC;
extern std::int16_t TELEPORT5ONPIC;
extern std::int16_t TELEPORT6ONPIC;
extern std::int16_t TELEPORT7ONPIC;
extern std::int16_t TELEPORT8ONPIC;
extern std::int16_t TELEPORT9ONPIC;
extern std::int16_t TELEPORT10ONPIC;
extern std::int16_t TELEPORT_TEXT_BG;
extern std::int16_t BACKGROUND_SCREENPIC;
extern std::int16_t APOGEEPIC;
extern std::int16_t PIRACYPIC;
extern std::int16_t PC13PIC;
extern std::int16_t LOSEPIC;
extern std::int16_t AUTOMAPPIC;
extern std::int16_t PSPROMO1PIC;
extern std::int16_t PSPROMO2PIC;
extern std::int16_t PSPROMO3PIC;
extern std::int16_t H_ALTPIC;
extern std::int16_t H_CTRLPIC;
extern std::int16_t H_SPACEPIC;
extern std::int16_t H_PAUSEPIC;
extern std::int16_t H_ESCPIC;
extern std::int16_t H_LTARROWPIC;
extern std::int16_t H_UPARROWPIC;
extern std::int16_t H_DNARROWPIC;
extern std::int16_t H_RTARROWPIC;
extern std::int16_t H_ENTERPIC;
extern std::int16_t H_QPIC;
extern std::int16_t H_WPIC;
extern std::int16_t H_EPIC;
extern std::int16_t H_IPIC;
extern std::int16_t H_HPIC;
extern std::int16_t H_1PIC;
extern std::int16_t H_2PIC;
extern std::int16_t H_3PIC;
extern std::int16_t H_4PIC;
extern std::int16_t H_5PIC;
extern std::int16_t H_F1PIC;
extern std::int16_t H_F2PIC;
extern std::int16_t H_F3PIC;
extern std::int16_t H_F4PIC;
extern std::int16_t H_F5PIC;
extern std::int16_t H_F6PIC;
extern std::int16_t H_F7PIC;
extern std::int16_t H_F8PIC;
extern std::int16_t H_F9PIC;
extern std::int16_t H_F10PIC;
extern std::int16_t H_TABPIC;
extern std::int16_t H_CPIC;
extern std::int16_t H_FPIC;
extern std::int16_t H_PPIC;
extern std::int16_t H_MPIC;
extern std::int16_t H_LPIC;
extern std::int16_t H_SHIFTPIC;
extern std::int16_t APOGEE_LOGOPIC;
extern std::int16_t VISAPIC;
extern std::int16_t MCPIC;
extern std::int16_t FAXPIC;
extern std::int16_t H_TOPWINDOWPIC;
extern std::int16_t H_LEFTWINDOWPIC;
extern std::int16_t H_RIGHTWINDOWPIC;
extern std::int16_t H_BOTTOMINFOPIC;
extern std::int16_t C_NOTSELECTEDPIC;
extern std::int16_t C_SELECTEDPIC;
extern std::int16_t C_NOTSELECTED_HIPIC;
extern std::int16_t C_SELECTED_HIPIC;
extern std::int16_t C_BABYMODEPIC;
extern std::int16_t C_EASYPIC;
extern std::int16_t C_NORMALPIC;
extern std::int16_t C_HARDPIC;
extern std::int16_t C_EPISODE1PIC;
extern std::int16_t C_EPISODE2PIC;
extern std::int16_t C_EPISODE3PIC;
extern std::int16_t C_EPISODE4PIC;
extern std::int16_t C_EPISODE5PIC;
extern std::int16_t C_EPISODE6PIC;
extern std::int16_t BIGGOLDSTERNPIC;
extern std::int16_t STARLOGOPIC;
extern std::int16_t BLAKEWITHGUNPIC;
extern std::int16_t STARINSTITUTEPIC;
extern std::int16_t MEDALOFHONORPIC;
extern std::int16_t SMALLGOLDSTERNPIC;
extern std::int16_t BLAKEWINPIC;
extern std::int16_t SHUTTLEEXPPIC;
extern std::int16_t PLANETSPIC;
extern std::int16_t MOUSEPIC;
extern std::int16_t JOYSTICKPIC;
extern std::int16_t GRAVISPADPIC;
extern std::int16_t TITLEPIC;
extern std::int16_t PROMO1PIC;
extern std::int16_t PROMO2PIC;
extern std::int16_t WEAPON1PIC;
extern std::int16_t WEAPON2PIC;
extern std::int16_t WEAPON3PIC;
extern std::int16_t WEAPON4PIC;
extern std::int16_t WEAPON5PIC;
extern std::int16_t WAITPIC;
extern std::int16_t READYPIC;
extern std::int16_t N_BLANKPIC;
extern std::int16_t N_0PIC;
extern std::int16_t N_1PIC;
extern std::int16_t N_2PIC;
extern std::int16_t N_3PIC;
extern std::int16_t N_4PIC;
extern std::int16_t N_5PIC;
extern std::int16_t N_6PIC;
extern std::int16_t N_7PIC;
extern std::int16_t N_8PIC;
extern std::int16_t N_9PIC;
extern std::int16_t N_RPIC;
extern std::int16_t N_OPIC;
extern std::int16_t N_LPIC;
extern std::int16_t N_DASHPIC;
extern std::int16_t DIM_LIGHTPIC;
extern std::int16_t BRI_LIGHTPIC;
extern std::int16_t ECG_HEART_GOOD;
extern std::int16_t ECG_HEART_BAD;
extern std::int16_t ECG_GRID_PIECE;
extern std::int16_t AMMONUM_BACKGR;
extern std::int16_t ECG_HEARTBEAT_00;
extern std::int16_t ECG_HEARTBEAT_01;
extern std::int16_t ECG_HEARTBEAT_02;
extern std::int16_t ECG_HEARTBEAT_03;
extern std::int16_t ECG_HEARTBEAT_04;
extern std::int16_t ECG_HEARTBEAT_05;
extern std::int16_t ECG_HEARTBEAT_06;
extern std::int16_t ECG_HEARTBEAT_07;
extern std::int16_t ECG_HEARTBEAT_08;
extern std::int16_t ECG_HEARTBEAT_09;
extern std::int16_t ECG_HEARTBEAT_10;
extern std::int16_t ECG_HEARTBEAT_11;
extern std::int16_t ECG_HEARTBEAT_12;
extern std::int16_t ECG_HEARTBEAT_13;
extern std::int16_t ECG_HEARTBEAT_14;
extern std::int16_t ECG_HEARTBEAT_15;
extern std::int16_t ECG_HEARTBEAT_16;
extern std::int16_t ECG_HEARTBEAT_17;
extern std::int16_t ECG_HEARTBEAT_18;
extern std::int16_t ECG_HEARTBEAT_19;
extern std::int16_t ECG_HEARTBEAT_20;
extern std::int16_t ECG_HEARTBEAT_21;
extern std::int16_t ECG_HEARTBEAT_22;
extern std::int16_t ECG_HEARTBEAT_23;
extern std::int16_t ECG_HEARTBEAT_24;
extern std::int16_t ECG_HEARTBEAT_25;
extern std::int16_t ECG_HEARTBEAT_26;
extern std::int16_t ECG_HEARTBEAT_27;
extern std::int16_t INFOAREAPIC;
extern std::int16_t TOP_STATUSBARPIC;
extern std::int16_t STATUSBARPIC;
extern std::int16_t PIRACYPALETTE;
extern std::int16_t APOGEEPALETTE;
extern std::int16_t TITLEPALETTE;
extern std::int16_t ORDERSCREEN;
extern std::int16_t ERRORSCREEN;
extern std::int16_t INFORMANT_HINTS;
extern std::int16_t NICE_SCIE_HINTS;
extern std::int16_t MEAN_SCIE_HINTS;
extern std::int16_t BRIEF_W1;
extern std::int16_t BRIEF_I1;
extern std::int16_t BRIEF_W2;
extern std::int16_t BRIEF_I2;
extern std::int16_t BRIEF_W3;
extern std::int16_t BRIEF_I3;
extern std::int16_t BRIEF_W4;
extern std::int16_t BRIEF_I4;
extern std::int16_t BRIEF_W5;
extern std::int16_t BRIEF_I5;
extern std::int16_t BRIEF_W6;
extern std::int16_t BRIEF_I6;
extern std::int16_t LEVEL_DESCS;
extern std::int16_t POWERBALLTEXT;
extern std::int16_t TICSTEXT;
extern std::int16_t MUSICTEXT;
extern std::int16_t RADARTEXT;
extern std::int16_t HELPTEXT;
extern std::int16_t SAGATEXT;
extern std::int16_t LOSETEXT;
extern std::int16_t ORDERTEXT;
extern std::int16_t CREDITSTEXT;
extern std::int16_t MUSTBE386TEXT;
extern std::int16_t QUICK_INFO1_TEXT;
extern std::int16_t QUICK_INFO2_TEXT;
extern std::int16_t BADINFO_TEXT;
extern std::int16_t CALJOY1_TEXT;
extern std::int16_t CALJOY2_TEXT;
extern std::int16_t READTHIS_TEXT;
extern std::int16_t ELEVMSG0_TEXT;
extern std::int16_t ELEVMSG1_TEXT;
extern std::int16_t ELEVMSG4_TEXT;
extern std::int16_t ELEVMSG5_TEXT;
extern std::int16_t FLOORMSG_TEXT;
extern std::int16_t YOUWIN_TEXT;
extern std::int16_t CHANGEVIEW_TEXT;
extern std::int16_t BADCHECKSUMTEXT;
extern std::int16_t DIZ_ERR_TEXT;
extern std::int16_t BADLEVELSTEXT;
extern std::int16_t BADSAVEGAME_TEXT;

extern std::int16_t TELEPORTBACKTOPPIC;
extern std::int16_t TELEPORTBACKBOTPIC;
extern std::int16_t TELEPORT11ONPIC;
extern std::int16_t TELEPORT12ONPIC;
extern std::int16_t TELEPORT13ONPIC;
extern std::int16_t TELEPORT14ONPIC;
extern std::int16_t TELEPORT15ONPIC;
extern std::int16_t TELEPORT16ONPIC;
extern std::int16_t TELEPORT17ONPIC;
extern std::int16_t TELEPORT18ONPIC;
extern std::int16_t TELEPORT19ONPIC;
extern std::int16_t TELEPORT20ONPIC;
extern std::int16_t TELEUPONPIC;
extern std::int16_t TELEDNONPIC;
extern std::int16_t TELEUPOFFPIC;
extern std::int16_t TELEDNOFFPIC;
extern std::int16_t TELEPORT11OFFPIC;
extern std::int16_t TELEPORT12OFFPIC;
extern std::int16_t TELEPORT13OFFPIC;
extern std::int16_t TELEPORT14OFFPIC;
extern std::int16_t TELEPORT15OFFPIC;
extern std::int16_t TELEPORT16OFFPIC;
extern std::int16_t TELEPORT17OFFPIC;
extern std::int16_t TELEPORT18OFFPIC;
extern std::int16_t TELEPORT19OFFPIC;
extern std::int16_t TELEPORT20OFFPIC;
extern std::int16_t AUTOMAP_MAG1PIC;
extern std::int16_t AUTOMAP_MAG2PIC;
extern std::int16_t AUTOMAP_MAG4PIC;
extern std::int16_t H_6PIC;
extern std::int16_t H_TILDEPIC;
extern std::int16_t H_PLUSPIC;
extern std::int16_t H_MINUSPIC;
extern std::int16_t STARPORTPIC;
extern std::int16_t BOSSPIC;
extern std::int16_t THREEPLANETSPIC;
extern std::int16_t SOLARSYSTEMPIC;
extern std::int16_t AOGENDINGPIC;
extern std::int16_t GFLOGOSPIC;
extern std::int16_t BLAKEHEADPIC;
extern std::int16_t PROJECTFOLDERPIC;
extern std::int16_t TITLE1PIC;
extern std::int16_t TITLE2PIC;
extern std::int16_t WEAPON6PIC;
extern std::int16_t WEAPON7PIC;
extern std::int16_t W1_CORNERPIC;
extern std::int16_t W2_CORNERPIC;
extern std::int16_t W3_CORNERPIC;
extern std::int16_t W4_CORNERPIC;
extern std::int16_t W5_CORNERPIC;
extern std::int16_t W6_CORNERPIC;
extern std::int16_t NG_BLANKPIC;
extern std::int16_t NG_0PIC;
extern std::int16_t NG_1PIC;
extern std::int16_t NG_2PIC;
extern std::int16_t NG_3PIC;
extern std::int16_t NG_4PIC;
extern std::int16_t NG_5PIC;
extern std::int16_t NG_6PIC;
extern std::int16_t NG_7PIC;
extern std::int16_t NG_8PIC;
extern std::int16_t NG_9PIC;
extern std::int16_t ONEXZOOMPIC;
extern std::int16_t TWOXZOOMPIC;
extern std::int16_t FOURXZOOMPIC;
extern std::int16_t NO_KEYPIC;
extern std::int16_t RED_KEYPIC;
extern std::int16_t YEL_KEYPIC;
extern std::int16_t BLU_KEYPIC;
extern std::int16_t ENDINGPALETTE;
extern std::int16_t NO386SCREEN;
extern std::int16_t T_DEMO0;
extern std::int16_t T_DEMO1;
extern std::int16_t T_DEMO2;
extern std::int16_t T_DEMO3;
extern std::int16_t T_DEMO4;
extern std::int16_t T_DEMO5;
extern std::int16_t DECOY;
extern std::int16_t DECOY2;
extern std::int16_t DECOY3;
extern std::int16_t DECOY4;


#endif // BSTONE_GFXV_INCLUDED
