#include <switch.h>
#include <dirent.h>
#include <fstream>

#include <borealis.hpp>
#include "About_tab.hpp"

namespace i18n = brls::i18n; // for loadTranslations() and getStr()
using namespace i18n::literals; // for _i18n

bool isAtmosphere = false;
bool isSXOS = false;
brls::SelectListItem* StatusItem;
brls::SelectListItem* StatusItem2;
brls::SelectListItem* StatusItem3;
brls::SelectListItem* StatusItem4;
brls::SelectListItem* StatusItem5;
brls::SelectListItem* StatusItem6;
brls::SelectListItem* StatusItem8;
brls::SelectListItem* StatusItem9;
Thread t0;

struct _original {
    uint8_t anisoSelect = 0;
    uint8_t lock30FPSSelect = 1;
    uint8_t shadowRes_DockedSelect = 5;
    uint8_t shadowRes_HandheldSelect = 3;
    uint8_t shadowDist_DockedSelect = 7;
    uint8_t shadowDist_HandheldSelect = 4;
    uint8_t renderingResSelect = 4;
    uint8_t windowResSelect = 0;
    float anisoValue = 1;
    uint8_t lock30FPSValue = 2;
    uint16_t shadowRes_DockedValue = 4096;
    uint16_t shadowRes_HandheldValue = 2048;
    float shadowDist_DockedValue = 0.8;
    float shadowDist_HandheldValue = 0.5;
    uint16_t renderingRes_H = 720;
    uint16_t renderingRes_W = 1280;
    uint16_t windowRes_H = 720;
    uint16_t windowRes_W = 1280;
    uint16_t portraitsRes_WH = 256;
};

_original original;

char pathVegetationMod[31][128] {	"01005420101DA000/romfs/data/plt/a1000.plt",
                                    "01005420101DA000/romfs/data/plt/c0000.plt",
                                    "01005420101DA000/romfs/data/plt/c0800.plt",
                                    "01005420101DA000/romfs/data/plt/c2800.plt",
                                    "01005420101DA000/romfs/data/plt/f1000.plt",
                                    "01005420101DA000/romfs/data/plt/f1010.plt",
                                    "01005420101DA000/romfs/data/plt/f2000.plt",
                                    "01005420101DA000/romfs/data/plt/m0600.plt",
                                    "01005420101DA000/romfs/data/plt/m1490.plt",
                                    "01005420101DA000/romfs/data/plt/m3040.plt",
                                    "01005420101DA000/romfs/data/plt/m3050.plt",
                                    "01005420101DA000/romfs/data/plt/r0010.plt",
                                    "01005420101DA000/romfs/data/plt/r0400.plt",
                                    "01005420101DA000/romfs/data/plt/r1000.plt",
                                    "01005420101DA000/romfs/data/plt/r1200.plt",
                                    "01005420101DA000/romfs/data/plt/r1400.plt",
                                    "01005420101DA000/romfs/data/plt/r1410.plt",
                                    "01005420101DA000/romfs/data/plt/r2400.plt",
                                    "01005420101DA000/romfs/data/plt/r2410.plt",
                                    "01005420101DA000/romfs/data/plt/r2420.plt",
                                    "01005420101DA000/romfs/data/plt/r3410.plt",
                                    "01005420101DA000/romfs/data/plt/r3600.plt",
                                    "01005420101DA000/romfs/data/plt/t0000.plt",
                                    "01005420101DA000/romfs/data/plt/t0200.plt",
                                    "01005420101DA000/romfs/data/plt/t0210.plt",
                                    "01005420101DA000/romfs/data/plt/t0240.plt",
                                    "01005420101DA000/romfs/data/plt/t0250.plt",
                                    "01005420101DA000/romfs/data/plt/t0270.plt",
                                    "01005420101DA000/romfs/data/plt/t0400.plt",
                                    "01005420101DA000/romfs/data/plt/t2000.plt",
                                    "01005420101DA000/romfs/data/plt/t2200.plt" };

char romfsVegetationMod[31][128];
char sxosVegetationMod[31][128];
char atmosphereVegetationMod[31][128];
										
void WritePaths() {
	for (uint8_t i = 0; i < 31; i++) {
		sprintf(romfsVegetationMod[i], "romfs:/%s", pathVegetationMod[i]);
		sprintf(sxosVegetationMod[i], "sdmc:/sxos/titles/%s", pathVegetationMod[i]);
		sprintf(atmosphereVegetationMod[i], "sdmc:/atmosphere/contents/%s", pathVegetationMod[i]);
	}
}

struct _offset {
    //Anisotropic filtering
    uint32_t aniso = 0x00956944;
    
    //30 FPS lock
    uint32_t windowbuilderinterval = 0x00965E94;
    uint32_t frameratecap = 0x00511918;
    
    //Docked shadow resolution
    uint32_t dockedshadowres = 0x007B8FE8;
    
    //Handheld shadow resolution
    uint32_t handheldshadowres = 0x007B8FF0;
    
    //Docked shadow distance
    uint32_t dockedshadowdist = 0x007B8FFC;
    
    //Handheld shadow distance
    uint32_t handheldshadowdist = 0x007B9000;
    
    //Rendering Resolution (handheld and docked are using the same rendering resolution)
    ///Width
    uint32_t renderRes_W0 = 0x002E4664;
    uint32_t renderRes_W1 = 0x002E4694;
    ///Height
    uint32_t renderRes_H0 = 0x002E466C;
    uint32_t renderRes_H1 = 0x002E4698;

    //Window Resolution (handheld and docked are using the same window resolution)
    uint32_t windowRes_WH = 0x00A52B60;
    ///Width
    uint32_t windowRes_W0 = 0x002E46D4;
    uint32_t windowRes_W1 = 0x002E47F0;
    uint32_t windowRes_W2 = 0x002E4838;
    uint32_t windowRes_W3 = 0x002E4880;
    uint32_t windowRes_W4 = 0x002E48AC;
    uint32_t windowRes_W5 = 0x002E4A0C;
    uint32_t windowRes_W6 = 0x002E4A38;
    uint32_t windowRes_W7 = 0x002E4A64;
    uint32_t windowRes_W8 = 0x002E4A90;
    uint32_t windowRes_W9 = 0x002E4ABC;
    uint32_t windowRes_WA = 0x002E4CE4;
    uint32_t windowRes_WB = 0x00965CCC;
    uint32_t windowRes_WC = 0x00966D9C;
    ///Height
    uint32_t windowRes_H0 = 0x002E46D8;
    uint32_t windowRes_H1 = 0x002E47F4;
    uint32_t windowRes_H2 = 0x002E483C;
    uint32_t windowRes_H3 = 0x002E4884;
    uint32_t windowRes_H4 = 0x002E48B0;
    uint32_t windowRes_H5 = 0x002E4A10;
    uint32_t windowRes_H6 = 0x002E4A3C;
    uint32_t windowRes_H7 = 0x002E4A68;
    uint32_t windowRes_H8 = 0x002E4A94;
    uint32_t windowRes_H9 = 0x002E4AC0;
    uint32_t windowRes_HA = 0x002E4CE8;
    uint32_t windowRes_HB = 0x00965CD0;
    uint32_t windowRes_HC = 0x00966DA0;
    ///Portraits Width
    uint32_t portraitsRes_W0 = 0x002E4AEC;
    uint32_t portraitsRes_W1 = 0x002E4B18;
    uint32_t portraitsRes_W2 = 0x002E4B70;
    ///Portraits Height
    uint32_t portraitsRes_H0 = 0x002E4AF0;
    uint32_t portraitsRes_H1 = 0x002E4B1C;
    uint32_t portraitsRes_H2 = 0x002E4B74;	
};

constexpr _offset offset;

struct _instructions {
    //Anisotropic filtering
    uint8_t aniso0[4] = {0x68, 0x4e, 0x40, 0xbd}; //ldr s8,[x19, #0x4c]
    uint8_t aniso2x[4] = {0x08, 0x10, 0x20, 0x1e}; //fmov s8,#2.0
    uint8_t aniso4x[4] = {0x08, 0x10, 0x22, 0x1e}; //fmov s8,#4.0
    uint8_t aniso8x[4] = {0x08, 0x10, 0x24, 0x1e}; //fmov s8,#8.0
    uint8_t aniso16x[4] = {0x08, 0x10, 0x26, 0x1e}; //fmov s8,#16.0
    
    //30 FPS lock
    uint8_t windowbuilderinterval2[4] = {0xe1, 0x03, 0x1f, 0x32}; //mov w1,#2
    uint8_t windowbuilderinterval1[4] = {0xe1, 0x03, 0x00, 0x32}; //mov w1,#1
    uint8_t frameratecap30[8] = {0xa9, 0x0a, 0x94, 0x52, 0x89, 0x3f, 0xa0, 0x72}; //mov w9,#0xa055; movk w9,#0x1fc, LSL #16 (mov w9, #33333333)
    uint8_t frameratecap60[8] = {0x49, 0x05, 0x8a, 0x52, 0xc9, 0x1f, 0xa0, 0x72}; //mov w9,#0x502a; movk w9,#0xfe, LSL #16 (mov w9, #16666666)
    
    //Docked shadow resolution
    uint8_t dockedshadowres512[4] = {0xe9, 0x03, 0x17, 0x32}; //mov w9,#512
    uint8_t dockedshadowres1024[4] = {0xe9, 0x03, 0x16, 0x32}; //mov w9,#1024
    uint8_t dockedshadowres1536[4] = {0x09, 0xc0, 0x80, 0x52}; //mov w9,#1536
    uint8_t dockedshadowres2048[4] = {0xe9, 0x03, 0x15, 0x32}; //mov w9,#2048
    uint8_t dockedshadowres3072[4] = {0x09, 0x80, 0x81, 0x52}; //mov w9,#3072
    uint8_t dockedshadowres4096[4] = {0xe9, 0x03, 0x14, 0x32}; //mov w9,#4096
    
    //Handheld shadow resolution
    uint8_t handheldshadowres512[4] = {0xe8, 0x03, 0x17, 0x32}; //mov w8,#512
    uint8_t handheldshadowres1024[4] = {0xe8, 0x03, 0x16, 0x32}; //mov w8,#1024
    uint8_t handheldshadowres1536[4] = {0x08, 0xc0, 0x80, 0x52}; //mov w8,#1536
    uint8_t handheldshadowres2048[4] = {0xe8, 0x03, 0x15, 0x32}; //mov w8,#2048
    uint8_t handheldshadowres3072[4] = {0x08, 0x80, 0x81, 0x52}; //mov w8,#3072
    uint8_t handheldshadowres4096[4] = {0xe8, 0x03, 0x14, 0x32}; //mov w8,#4096
    
    //Docked shadow distance
    uint8_t dockedshadowdist_01[4] = {0x02, 0x10, 0x28, 0x1e}; //fmov s2,#0.125
    uint8_t dockedshadowdist_02[4] = {0x02, 0x50, 0x29, 0x1e}; //fmov s2,#0.203125
    uint8_t dockedshadowdist_03[4] = {0x02, 0x90, 0x2a, 0x1e}; //fmov s2,#0.3125
    uint8_t dockedshadowdist_04[4] = {0x02, 0x50, 0x2b, 0x1e}; //fmov s2,#0.40625
    uint8_t dockedshadowdist_05[4] = {0x02, 0x10, 0x2c, 0x1e}; //fmov s2,#0.5
    uint8_t dockedshadowdist_06[4] = {0x02, 0x70, 0x2c, 0x1e}; //fmov s2,#0.59375
    uint8_t dockedshadowdist_07[4] = {0x02, 0xd0, 0x2c, 0x1e}; //fmov s2,#0.6875
    uint8_t dockedshadowdist_08[4] = {0x22, 0x91, 0x4b, 0xbd}; //ldr s2,[x9, #0xb90]
    uint8_t dockedshadowdist_09[4] = {0x02, 0xb0, 0x2d, 0x1e}; //fmov s2,#0.90625
    uint8_t dockedshadowdist_10[4] = {0x02, 0x10, 0x2e, 0x1e}; //fmov s2,#1.0
    
    //Handheld shadow distance
    uint8_t handheldshadowdist_01[4] = {0x01, 0x10, 0x28, 0x1e}; //fmov s1,#0.125
    uint8_t handheldshadowdist_02[4] = {0x01, 0x50, 0x29, 0x1e}; //fmov s1,#0.203125
    uint8_t handheldshadowdist_03[4] = {0x01, 0x90, 0x2a, 0x1e}; //fmov s1,#0.3125
    uint8_t handheldshadowdist_04[4] = {0x01, 0x50, 0x2b, 0x1e}; //fmov s1,#0.40625
    uint8_t handheldshadowdist_05[4] = {0x01, 0x10, 0x2c, 0x1e}; //fmov s1,#0.5
    uint8_t handheldshadowdist_06[4] = {0x01, 0x70, 0x2c, 0x1e}; //fmov s1,#0.59375
    uint8_t handheldshadowdist_07[4] = {0x01, 0xd0, 0x2c, 0x1e}; //fmov s1,#0.6875
    uint8_t handheldshadowdist_08[4] = {0x21, 0x91, 0x4b, 0xbd}; //ldr s1,[x9, #0xb90]
    uint8_t handheldshadowdist_09[4] = {0x01, 0xb0, 0x2d, 0x1e}; //fmov s1,#0.90625
    uint8_t handheldshadowdist_10[4] = {0x01, 0x10, 0x2e, 0x1e}; //fmov s1,#1.0
    
    //Rendering Resolution (handheld and docked are using the same rendering resolution)
    ///Width
    uint8_t renderRes_WX_640[4] = {0x01, 0x50, 0x80, 0x52}; //mov w1,#640
    uint8_t renderRes_WX_800[4] = {0x01, 0x64, 0x80, 0x52}; //mov w1,#800
    uint8_t renderRes_WX_960[4] = {0x01, 0x78, 0x80, 0x52}; //mov w1,#960
    uint8_t renderRes_WX_1120[4] = {0x01, 0x8c, 0x80, 0x52}; //mov w1,#1120
    uint8_t renderRes_WX_1280[4] = {0x01, 0xa0, 0x80, 0x52}; //mov w1,#1280
    uint8_t renderRes_WX_1440[4] = {0x01, 0xb4, 0x80, 0x52}; //mov w1,#1440
    uint8_t renderRes_WX_1600[4] = {0x01, 0xc8, 0x80, 0x52}; //mov w1,#1600
    uint8_t renderRes_WX_1760[4] = {0x01, 0xDC, 0x80, 0x52}; //mov w1,#1760
    uint8_t renderRes_WX_1920[4] = {0x01, 0xf0, 0x80, 0x52}; //mov w1,#1920
    ///Height
    uint8_t renderRes_HX_360[4] = {0x02, 0x2d, 0x80, 0x52}; //mov w2,#360
    uint8_t renderRes_HX_450[4] = {0x42, 0x38, 0x80, 0x52}; //mov w2,#450
    uint8_t renderRes_HX_540[4] = {0x82, 0x43, 0x80, 0x52}; //mov w2,#540
    uint8_t renderRes_HX_630[4] = {0xC2, 0x4E, 0x80, 0x52}; //mov w2,#630
    uint8_t renderRes_HX_720[4] = {0x02, 0x5a, 0x80, 0x52}; //mov w2,#720
    uint8_t renderRes_HX_810[4] = {0x42, 0x65, 0x80, 0x52}; //mov w2,#810
    uint8_t renderRes_HX_900[4] = {0x82, 0x70, 0x80, 0x52}; //mov w2,#900
    uint8_t renderRes_HX_990[4] = {0xC2, 0x7B, 0x80, 0x52}; //mov w2,#990
    uint8_t renderRes_HX_1080[4] = {0x02, 0x87, 0x80, 0x52}; //mov w2,#1080

    //Window Resolution (handheld and docked are using the same window resolution)
    uint8_t windowRes_WH_1280_720[8] = {0x00, 0x05, 0x00, 0x00, 0xd0, 0x02, 0x00, 0x00}; //u32 1280, u32 720
    uint8_t windowRes_WH_1920_1080[8] = {0x80, 0x07, 0x00, 0x00, 0x38, 0x04, 0x00, 0x00}; //u32 1920, u32 1080
    ///Width
    uint8_t windowRes_WX_1280[4] = {0x01, 0xa0, 0x80, 0x52}; //mov w1,#1280
    uint8_t windowRes_WX_1920[4] = {0x01, 0xf0, 0x80, 0x52}; //mov w1,#1920

    ///Height
    uint8_t windowRes_HX_720[4] = {0x02, 0x5a, 0x80, 0x52}; //mov w2,#720
    uint8_t windowRes_HX_1080[4] = {0x02, 0x87, 0x80, 0x52}; //mov w2,#1080
    ///Portraits Width
    uint8_t portraitsRes_WX_256[4] = {0x01, 0x20, 0x80, 0x52}; //mov w1,#256
    uint8_t portraitsRes_WX_1024[4] = {0x01, 0x80, 0x80, 0x52}; //mov w1,#1024
    ///Portraits Height
    uint8_t portraitsRes_HX_256[4] = {0x02, 0x20, 0x80, 0x52}; //mov w2,#256
    uint8_t portraitsRes_HX_1024[4] = {0x02, 0x80, 0x80, 0x52}; //mov w2,#1024
};

constexpr _instructions Instructions;

uint8_t anisoSelect = 0;
uint8_t lock30FPSSelect = 1;
uint8_t shadowRes_DockedSelect = 5;
uint8_t shadowRes_HandheldSelect = 3;
uint8_t shadowDist_DockedSelect = 7;
uint8_t shadowDist_HandheldSelect = 4;
uint8_t renderingResSelect = 4;
uint8_t windowResSelect = 0;
uint32_t pointers[44];

FILE* config;

void ResetToDefault() {
    anisoSelect = original.anisoSelect;
    lock30FPSSelect = original.lock30FPSSelect;
    shadowRes_DockedSelect = original.shadowRes_DockedSelect;
    shadowRes_HandheldSelect = original.shadowRes_HandheldSelect;
    shadowDist_DockedSelect = original.shadowDist_DockedSelect;
    shadowDist_HandheldSelect = original.shadowDist_HandheldSelect;
    renderingResSelect = original.renderingResSelect;
    windowResSelect = original.windowResSelect;
    StatusItem->setSelectedValue(anisoSelect);
    StatusItem2->setSelectedValue(lock30FPSSelect);
    StatusItem3->setSelectedValue(shadowRes_DockedSelect);
    StatusItem4->setSelectedValue(shadowRes_HandheldSelect);
    StatusItem5->setSelectedValue(shadowDist_DockedSelect);
    StatusItem6->setSelectedValue(shadowDist_HandheldSelect);
    StatusItem8->setSelectedValue(renderingResSelect);
    StatusItem9->setSelectedValue(windowResSelect);	
    if (isAtmosphere) remove("sdmc:/atmosphere/exefs_patches/ToCS3GraphicsConfig/134EC3D8BE75126FF9BB49462A0E781465A675CA000000000000000000000000.ips");
    else if (isSXOS) remove("sdmc:/sxos/exefs_patches/ToCS3GraphicsConfig/134EC3D8BE75126FF9BB49462A0E781465A675CA000000000000000000000000.ips");
    brls::Application::notify("main/ResetSaved"_i18n);
}

size_t fwrite_bswap32(uint32_t value, FILE *stream) {
    uint32_t temp = __builtin_bswap32(value);
    uint16_t temp_size = __builtin_bswap16(0x4);
    fwrite(&temp, 4, 1, stream);
    return fwrite(&temp_size, 2, 1, stream);
}

size_t fwrite_offset_size_array(const uint32_t offset, const size_t size, const size_t nmemb, FILE *stream, const void* arrayPtr) {
    uint32_t temp = __builtin_bswap32(offset);
    uint16_t temp_size = __builtin_bswap16(nmemb);
    fwrite(&temp, 4, 1, stream);
    fwrite(&temp_size, 2, 1, stream);
    return fwrite(arrayPtr, size, nmemb, stream);
}

void copyFile(const char* frompath, const char* topath) {
	FILE *in = fopen(frompath, "rb");
	FILE *out = fopen(topath, "wb");

	char* buf = (char*)malloc(0x100000);
	size_t read = 0;

	//  Read data in 32kb chunks and write to output file
//	while ((read = fread(buf, 1, 0x100000, in)) == 0x100000)
//	{
//		fwrite(buf, 1, 0x100000, out);
//	}

	read = fread(buf, 1, 0x100000, in);

	//  If there is any data left over write it out
	fwrite(buf, 1, read, out);
	
	fclose(out);
	fclose(in);
	free(buf);
}

void InstallVegetationMod(void*) {
    if (isAtmosphere) {
        mkdir("sdmc:/atmosphere/contents/01005420101DA000/", 777);
        mkdir("sdmc:/atmosphere/contents/01005420101DA000/romfs", 777);
        mkdir("sdmc:/atmosphere/contents/01005420101DA000/romfs/data", 777);
        mkdir("sdmc:/atmosphere/contents/01005420101DA000/romfs/data/plt", 777);
        for (uint8_t i = 0; i < 31; i++) {
            copyFile(romfsVegetationMod[i], atmosphereVegetationMod[i]);
        }
    }
    else if (isSXOS) {
        mkdir("sdmc:/sxos", 777);
        mkdir("sdmc:/sxos/titles", 777);
        mkdir("sdmc:/sxos/titles/01005420101DA000/", 777);
        mkdir("sdmc:/sxos/titles/01005420101DA000/romfs", 777);
        mkdir("sdmc:/sxos/titles/01005420101DA000/romfs/data", 777);
        mkdir("sdmc:/sxos/titles/01005420101DA000/romfs/data/plt", 777);
        for (uint8_t i = 0; i < 31; i++) {
            copyFile(romfsVegetationMod[i], sxosVegetationMod[i]);
        }
    }
    brls::Application::notify("main/mods/notify/VegetationEnd"_i18n);
    threadExit();
}

void removeVegetationMod(void*) {
    if (isAtmosphere) for (uint8_t i = 0; i < 31; i++) {
        remove(atmosphereVegetationMod[i]);
    }
    else if (isSXOS) for (uint8_t i = 0; i < 31; i++) {
        remove(sxosVegetationMod[i]);
    }
    brls::Application::notify("main/mods/notify/VegetationRemoveEnd"_i18n);
    threadExit();
}

bool isVegetationMod() {
    if (isAtmosphere) for (uint8_t i = 0; i < 31; i++) {
        FILE* check = fopen(atmosphereVegetationMod[i], "rb");
        if (!check) return false;
        else fclose(check);
    }
    else if (isSXOS) for (uint8_t i = 0; i < 31; i++) {
        FILE* check = fopen(sxosVegetationMod[i], "rb");
        if (!check) return false;
        else fclose(check);
    }
    return true;
}

void WritetoArrays() {
    pointers[0] = offset.aniso;
    pointers[1] = offset.windowbuilderinterval;
    pointers[2] = offset.frameratecap;
    pointers[3] = offset.dockedshadowres;
    pointers[4] = offset.handheldshadowres;
    pointers[5] = offset.dockedshadowdist;
    pointers[6] = offset.handheldshadowdist;
    pointers[7] = offset.renderRes_W0;
    pointers[8] = offset.renderRes_W1;
    pointers[9] = offset.renderRes_H0;
    pointers[10] = offset.renderRes_H1;
    pointers[11] = offset.windowRes_WH;
    pointers[12] = offset.windowRes_W0;
    pointers[13] = offset.windowRes_W1;
    pointers[14] = offset.windowRes_W2;
    pointers[15] = offset.windowRes_W3;
    pointers[16] = offset.windowRes_W4;
    pointers[17] = offset.windowRes_W5;
    pointers[18] = offset.windowRes_W6;
    pointers[19] = offset.windowRes_W7;
    pointers[20] = offset.windowRes_W8;
    pointers[21] = offset.windowRes_W9;
    pointers[22] = offset.windowRes_WA;
    pointers[23] = offset.windowRes_WB;
    pointers[24] = offset.windowRes_WC;
    pointers[25] = offset.windowRes_H0;
    pointers[26] = offset.windowRes_H1;
    pointers[27] = offset.windowRes_H2;
    pointers[28] = offset.windowRes_H3;
    pointers[29] = offset.windowRes_H4;
    pointers[30] = offset.windowRes_H5;
    pointers[31] = offset.windowRes_H6;
    pointers[32] = offset.windowRes_H7;
    pointers[33] = offset.windowRes_H8;
    pointers[34] = offset.windowRes_H9;
    pointers[35] = offset.windowRes_HA;
    pointers[36] = offset.windowRes_HB;
    pointers[37] = offset.windowRes_HC;
    pointers[38] = offset.portraitsRes_W0;
    pointers[39] = offset.portraitsRes_W1;
    pointers[40] = offset.portraitsRes_W2;
    pointers[41] = offset.portraitsRes_H0;
    pointers[42] = offset.portraitsRes_H1;
    pointers[43] = offset.portraitsRes_H2;
}

uint8_t SaveSettings() {
    if (isAtmosphere) config = fopen("sdmc:/atmosphere/exefs_patches/ToCS3GraphicsConfig/134EC3D8BE75126FF9BB49462A0E781465A675CA000000000000000000000000.ips", "wb");
    else if (isSXOS) config = fopen("sdmc:/sxos/exefs_patches/ToCS3GraphicsConfig/134EC3D8BE75126FF9BB49462A0E781465A675CA000000000000000000000000.ips", "wb");
    char MAGIC[6] = "IPS32";
    fwrite(&MAGIC, 1, 5, config);
    fwrite_bswap32(offset.aniso, config);
    switch (anisoSelect) {
        case 0:
            fwrite(&Instructions.aniso0, 1, 4, config);
            break;
        case 1:
            fwrite(&Instructions.aniso2x, 1, 4, config);
            break;			
        case 2:
            fwrite(&Instructions.aniso4x, 1, 4, config);
            break;
        case 3:
            fwrite(&Instructions.aniso8x, 1, 4, config);
            break;
        case 4:
            fwrite(&Instructions.aniso16x, 1, 4, config);
            break;
        
        default:
            return 1;
            break;
    }
    
    fwrite_bswap32(offset.windowbuilderinterval, config);
    switch (lock30FPSSelect) {
        case 0:
            fwrite(&Instructions.windowbuilderinterval1, 1, 4, config);
            break;
        case 1:
            fwrite(&Instructions.windowbuilderinterval2, 1, 4, config);
            break;			
        
        default:
            return 1;
            break;
    }

    switch (lock30FPSSelect) {
        case 0:
			fwrite_offset_size_array(offset.frameratecap, 1, 8, config, &Instructions.frameratecap60);
            break;
        case 1:
            fwrite_offset_size_array(offset.frameratecap, 1, 8, config, &Instructions.frameratecap30);
            break;			
        
        default:
            return 1;
            break;
    }
    
    fwrite_bswap32(offset.dockedshadowres, config);
    switch (shadowRes_DockedSelect) {
        case 0:
            fwrite(&Instructions.dockedshadowres512, 1, 4, config);
            break;
        case 1:
            fwrite(&Instructions.dockedshadowres1024, 1, 4, config);
            break;			
        case 2:
            fwrite(&Instructions.dockedshadowres1536, 1, 4, config);
            break;			
        case 3:
            fwrite(&Instructions.dockedshadowres2048, 1, 4, config);
            break;			
        case 4:
            fwrite(&Instructions.dockedshadowres3072, 1, 4, config);
            break;			
        case 5:
            fwrite(&Instructions.dockedshadowres4096, 1, 4, config);
            break;						

        default:
            return 1;
            break;
    }
    
    fwrite_bswap32(offset.handheldshadowres, config);
    switch (shadowRes_HandheldSelect) {
        case 0:
            fwrite(&Instructions.handheldshadowres512, 1, 4, config);
            break;
        case 1:
            fwrite(&Instructions.handheldshadowres1024, 1, 4, config);
            break;			
        case 2:
            fwrite(&Instructions.handheldshadowres1536, 1, 4, config);
            break;			
        case 3:
            fwrite(&Instructions.handheldshadowres2048, 1, 4, config);
            break;			
        case 4:
            fwrite(&Instructions.handheldshadowres3072, 1, 4, config);
            break;			
        case 5:
            fwrite(&Instructions.handheldshadowres4096, 1, 4, config);
            break;						

        default:
            return 1;
            break;
    }
    
    fwrite_bswap32(offset.dockedshadowdist, config);
    switch (shadowDist_DockedSelect) {
        case 0:
            fwrite(&Instructions.dockedshadowdist_01, 1, 4, config);
            break;
        case 1:
            fwrite(&Instructions.dockedshadowdist_02, 1, 4, config);
            break;			
        case 2:
            fwrite(&Instructions.dockedshadowdist_03, 1, 4, config);
            break;			
        case 3:
            fwrite(&Instructions.dockedshadowdist_04, 1, 4, config);
            break;			
        case 4:
            fwrite(&Instructions.dockedshadowdist_05, 1, 4, config);
            break;			
        case 5:
            fwrite(&Instructions.dockedshadowdist_06, 1, 4, config);
            break;						
        case 6:
            fwrite(&Instructions.dockedshadowdist_07, 1, 4, config);
            break;						
        case 7:
            fwrite(&Instructions.dockedshadowdist_08, 1, 4, config);
            break;						
        case 8:
            fwrite(&Instructions.dockedshadowdist_09, 1, 4, config);
            break;						
        case 9:
            fwrite(&Instructions.dockedshadowdist_10, 1, 4, config);
            break;						

        default:
            return 1;
            break;
    }

    fwrite_bswap32(offset.handheldshadowdist, config);
    switch (shadowDist_HandheldSelect) {
        case 0:
            fwrite(&Instructions.handheldshadowdist_01, 1, 4, config);
            break;
        case 1:
            fwrite(&Instructions.handheldshadowdist_02, 1, 4, config);
            break;			
        case 2:
            fwrite(&Instructions.handheldshadowdist_03, 1, 4, config);
            break;			
        case 3:
            fwrite(&Instructions.handheldshadowdist_04, 1, 4, config);
            break;			
        case 4:
            fwrite(&Instructions.handheldshadowdist_05, 1, 4, config);
            break;			
        case 5:
            fwrite(&Instructions.handheldshadowdist_06, 1, 4, config);
            break;						
        case 6:
            fwrite(&Instructions.handheldshadowdist_07, 1, 4, config);
            break;						
        case 7:
            fwrite(&Instructions.handheldshadowdist_08, 1, 4, config);
            break;						
        case 8:
            fwrite(&Instructions.handheldshadowdist_09, 1, 4, config);
            break;						
        case 9:
            fwrite(&Instructions.handheldshadowdist_10, 1, 4, config);
            break;						

        default:
            return 1;
            break;
    }
    
    switch (renderingResSelect) {
        case 0:
            fwrite_offset_size_array(offset.renderRes_W0, 1, 4, config, &Instructions.renderRes_WX_640);
            fwrite_offset_size_array(offset.renderRes_W1, 1, 4, config, &Instructions.renderRes_WX_640);
            fwrite_offset_size_array(offset.renderRes_H0, 1, 4, config, &Instructions.renderRes_HX_360);
            fwrite_offset_size_array(offset.renderRes_H1, 1, 4, config, &Instructions.renderRes_HX_360);
            break;

        case 1:
            fwrite_offset_size_array(offset.renderRes_W0, 1, 4, config, &Instructions.renderRes_WX_800);
            fwrite_offset_size_array(offset.renderRes_W1, 1, 4, config, &Instructions.renderRes_WX_800);
            fwrite_offset_size_array(offset.renderRes_H0, 1, 4, config, &Instructions.renderRes_HX_450);
            fwrite_offset_size_array(offset.renderRes_H1, 1, 4, config, &Instructions.renderRes_HX_450);
            break;
			
        case 2:
            fwrite_offset_size_array(offset.renderRes_W0, 1, 4, config, &Instructions.renderRes_WX_960);
            fwrite_offset_size_array(offset.renderRes_W1, 1, 4, config, &Instructions.renderRes_WX_960);
            fwrite_offset_size_array(offset.renderRes_H0, 1, 4, config, &Instructions.renderRes_HX_540);
            fwrite_offset_size_array(offset.renderRes_H1, 1, 4, config, &Instructions.renderRes_HX_540);
            break;
        
		case 3:
            fwrite_offset_size_array(offset.renderRes_W0, 1, 4, config, &Instructions.renderRes_WX_1120);
            fwrite_offset_size_array(offset.renderRes_W1, 1, 4, config, &Instructions.renderRes_WX_1120);
            fwrite_offset_size_array(offset.renderRes_H0, 1, 4, config, &Instructions.renderRes_HX_630);
            fwrite_offset_size_array(offset.renderRes_H1, 1, 4, config, &Instructions.renderRes_HX_630);
            break;
		
        case 4:
            fwrite_offset_size_array(offset.renderRes_W0, 1, 4, config, &Instructions.renderRes_WX_1280);
            fwrite_offset_size_array(offset.renderRes_W1, 1, 4, config, &Instructions.renderRes_WX_1280);
            fwrite_offset_size_array(offset.renderRes_H0, 1, 4, config, &Instructions.renderRes_HX_720);
            fwrite_offset_size_array(offset.renderRes_H1, 1, 4, config, &Instructions.renderRes_HX_720);
            break;
            
        case 5:
            fwrite_offset_size_array(offset.renderRes_W0, 1, 4, config, &Instructions.renderRes_WX_1440);
            fwrite_offset_size_array(offset.renderRes_W1, 1, 4, config, &Instructions.renderRes_WX_1440);
            fwrite_offset_size_array(offset.renderRes_H0, 1, 4, config, &Instructions.renderRes_HX_810);
            fwrite_offset_size_array(offset.renderRes_H1, 1, 4, config, &Instructions.renderRes_HX_810);
            break;
        
        case 6:
            fwrite_offset_size_array(offset.renderRes_W0, 1, 4, config, &Instructions.renderRes_WX_1600);
            fwrite_offset_size_array(offset.renderRes_W1, 1, 4, config, &Instructions.renderRes_WX_1600);
            fwrite_offset_size_array(offset.renderRes_H0, 1, 4, config, &Instructions.renderRes_HX_900);
            fwrite_offset_size_array(offset.renderRes_H1, 1, 4, config, &Instructions.renderRes_HX_900);
            break;

        case 7:
            fwrite_offset_size_array(offset.renderRes_W0, 1, 4, config, &Instructions.renderRes_WX_1760);
            fwrite_offset_size_array(offset.renderRes_W1, 1, 4, config, &Instructions.renderRes_WX_1760);
            fwrite_offset_size_array(offset.renderRes_H0, 1, 4, config, &Instructions.renderRes_HX_990);
            fwrite_offset_size_array(offset.renderRes_H1, 1, 4, config, &Instructions.renderRes_HX_990);
            break;
			
        case 8:
            fwrite_offset_size_array(offset.renderRes_W0, 1, 4, config, &Instructions.renderRes_WX_1920);
            fwrite_offset_size_array(offset.renderRes_W1, 1, 4, config, &Instructions.renderRes_WX_1920);
            fwrite_offset_size_array(offset.renderRes_H0, 1, 4, config, &Instructions.renderRes_HX_1080);
            fwrite_offset_size_array(offset.renderRes_H1, 1, 4, config, &Instructions.renderRes_HX_1080);
            break;
        
        default:
            return 1;
            break;
    }
    
    switch (windowResSelect) {
        case 0:
            fwrite_offset_size_array(offset.windowRes_WH, 1, 8, config, &Instructions.windowRes_WH_1280_720);
            fwrite_offset_size_array(offset.windowRes_W0, 1, 4, config, &Instructions.windowRes_WX_1280);
            fwrite_offset_size_array(offset.windowRes_W1, 1, 4, config, &Instructions.windowRes_WX_1280);
            fwrite_offset_size_array(offset.windowRes_W2, 1, 4, config, &Instructions.windowRes_WX_1280);
            fwrite_offset_size_array(offset.windowRes_W3, 1, 4, config, &Instructions.windowRes_WX_1280);
            fwrite_offset_size_array(offset.windowRes_W4, 1, 4, config, &Instructions.windowRes_WX_1280);
            fwrite_offset_size_array(offset.windowRes_W5, 1, 4, config, &Instructions.windowRes_WX_1280);
            fwrite_offset_size_array(offset.windowRes_W6, 1, 4, config, &Instructions.windowRes_WX_1280);
            fwrite_offset_size_array(offset.windowRes_W7, 1, 4, config, &Instructions.windowRes_WX_1280);
            fwrite_offset_size_array(offset.windowRes_W8, 1, 4, config, &Instructions.windowRes_WX_1280);
            fwrite_offset_size_array(offset.windowRes_W9, 1, 4, config, &Instructions.windowRes_WX_1280);
            fwrite_offset_size_array(offset.windowRes_WA, 1, 4, config, &Instructions.windowRes_WX_1280);
            fwrite_offset_size_array(offset.windowRes_WB, 1, 4, config, &Instructions.windowRes_WX_1280);
            fwrite_offset_size_array(offset.windowRes_WC, 1, 4, config, &Instructions.windowRes_WX_1280);
            fwrite_offset_size_array(offset.windowRes_H0, 1, 4, config, &Instructions.windowRes_HX_720);
            fwrite_offset_size_array(offset.windowRes_H1, 1, 4, config, &Instructions.windowRes_HX_720);
            fwrite_offset_size_array(offset.windowRes_H2, 1, 4, config, &Instructions.windowRes_HX_720);
            fwrite_offset_size_array(offset.windowRes_H3, 1, 4, config, &Instructions.windowRes_HX_720);
            fwrite_offset_size_array(offset.windowRes_H4, 1, 4, config, &Instructions.windowRes_HX_720);
            fwrite_offset_size_array(offset.windowRes_H5, 1, 4, config, &Instructions.windowRes_HX_720);
            fwrite_offset_size_array(offset.windowRes_H6, 1, 4, config, &Instructions.windowRes_HX_720);
            fwrite_offset_size_array(offset.windowRes_H7, 1, 4, config, &Instructions.windowRes_HX_720);
            fwrite_offset_size_array(offset.windowRes_H8, 1, 4, config, &Instructions.windowRes_HX_720);
            fwrite_offset_size_array(offset.windowRes_H9, 1, 4, config, &Instructions.windowRes_HX_720);
            fwrite_offset_size_array(offset.windowRes_HA, 1, 4, config, &Instructions.windowRes_HX_720);
            fwrite_offset_size_array(offset.windowRes_HB, 1, 4, config, &Instructions.windowRes_HX_720);
            fwrite_offset_size_array(offset.windowRes_HC, 1, 4, config, &Instructions.windowRes_HX_720);
            fwrite_offset_size_array(offset.portraitsRes_W0, 1, 4, config, &Instructions.portraitsRes_WX_256);
            fwrite_offset_size_array(offset.portraitsRes_W1, 1, 4, config, &Instructions.portraitsRes_WX_256);
            fwrite_offset_size_array(offset.portraitsRes_W2, 1, 4, config, &Instructions.portraitsRes_WX_256);
            fwrite_offset_size_array(offset.portraitsRes_H0, 1, 4, config, &Instructions.portraitsRes_HX_256);
            fwrite_offset_size_array(offset.portraitsRes_H1, 1, 4, config, &Instructions.portraitsRes_HX_256);
            fwrite_offset_size_array(offset.portraitsRes_H2, 1, 4, config, &Instructions.portraitsRes_HX_256);
            break;
        
        case 1:
            fwrite_offset_size_array(offset.windowRes_WH, 1, 8, config, &Instructions.windowRes_WH_1920_1080);
            fwrite_offset_size_array(offset.windowRes_W0, 1, 4, config, &Instructions.windowRes_WX_1920);
            fwrite_offset_size_array(offset.windowRes_W1, 1, 4, config, &Instructions.windowRes_WX_1920);
            fwrite_offset_size_array(offset.windowRes_W2, 1, 4, config, &Instructions.windowRes_WX_1920);
            fwrite_offset_size_array(offset.windowRes_W3, 1, 4, config, &Instructions.windowRes_WX_1920);
            fwrite_offset_size_array(offset.windowRes_W4, 1, 4, config, &Instructions.windowRes_WX_1920);
            fwrite_offset_size_array(offset.windowRes_W5, 1, 4, config, &Instructions.windowRes_WX_1920);
            fwrite_offset_size_array(offset.windowRes_W6, 1, 4, config, &Instructions.windowRes_WX_1920);
            fwrite_offset_size_array(offset.windowRes_W7, 1, 4, config, &Instructions.windowRes_WX_1920);
            fwrite_offset_size_array(offset.windowRes_W8, 1, 4, config, &Instructions.windowRes_WX_1920);
            fwrite_offset_size_array(offset.windowRes_W9, 1, 4, config, &Instructions.windowRes_WX_1920);
            fwrite_offset_size_array(offset.windowRes_WA, 1, 4, config, &Instructions.windowRes_WX_1920);
            fwrite_offset_size_array(offset.windowRes_WB, 1, 4, config, &Instructions.windowRes_WX_1920);
            fwrite_offset_size_array(offset.windowRes_WC, 1, 4, config, &Instructions.windowRes_WX_1920);
            fwrite_offset_size_array(offset.windowRes_H0, 1, 4, config, &Instructions.windowRes_HX_1080);
            fwrite_offset_size_array(offset.windowRes_H1, 1, 4, config, &Instructions.windowRes_HX_1080);
            fwrite_offset_size_array(offset.windowRes_H2, 1, 4, config, &Instructions.windowRes_HX_1080);
            fwrite_offset_size_array(offset.windowRes_H3, 1, 4, config, &Instructions.windowRes_HX_1080);
            fwrite_offset_size_array(offset.windowRes_H4, 1, 4, config, &Instructions.windowRes_HX_1080);
            fwrite_offset_size_array(offset.windowRes_H5, 1, 4, config, &Instructions.windowRes_HX_1080);
            fwrite_offset_size_array(offset.windowRes_H6, 1, 4, config, &Instructions.windowRes_HX_1080);
            fwrite_offset_size_array(offset.windowRes_H7, 1, 4, config, &Instructions.windowRes_HX_1080);
            fwrite_offset_size_array(offset.windowRes_H8, 1, 4, config, &Instructions.windowRes_HX_1080);
            fwrite_offset_size_array(offset.windowRes_H9, 1, 4, config, &Instructions.windowRes_HX_1080);
            fwrite_offset_size_array(offset.windowRes_HA, 1, 4, config, &Instructions.windowRes_HX_1080);
            fwrite_offset_size_array(offset.windowRes_HB, 1, 4, config, &Instructions.windowRes_HX_1080);
            fwrite_offset_size_array(offset.windowRes_HC, 1, 4, config, &Instructions.windowRes_HX_1080);
            fwrite_offset_size_array(offset.portraitsRes_W0, 1, 4, config, &Instructions.portraitsRes_WX_1024);
            fwrite_offset_size_array(offset.portraitsRes_W1, 1, 4, config, &Instructions.portraitsRes_WX_1024);
            fwrite_offset_size_array(offset.portraitsRes_W2, 1, 4, config, &Instructions.portraitsRes_WX_1024);
            fwrite_offset_size_array(offset.portraitsRes_H0, 1, 4, config, &Instructions.portraitsRes_HX_1024);
            fwrite_offset_size_array(offset.portraitsRes_H1, 1, 4, config, &Instructions.portraitsRes_HX_1024);
            fwrite_offset_size_array(offset.portraitsRes_H2, 1, 4, config, &Instructions.portraitsRes_HX_1024);
            break;

        default:
            return 1;
            break;
    }
    char EEOF[5] = "EEOF";
    fwrite(&EEOF, 1, 4, config);
    fclose(config);
    return 0;
}

uint8_t ReadSettings() {
    WritetoArrays();
    if (isAtmosphere) config = fopen("sdmc:/atmosphere/exefs_patches/ToCS3GraphicsConfig/134EC3D8BE75126FF9BB49462A0E781465A675CA000000000000000000000000.ips", "rb");
    else if (isSXOS) config = fopen("sdmc:/sxos/exefs_patches/ToCS3GraphicsConfig/134EC3D8BE75126FF9BB49462A0E781465A675CA000000000000000000000000.ips", "rb");
    
    if (!config) {
        if (isAtmosphere) mkdir("sdmc:/atmosphere/exefs_patches/ToCS3GraphicsConfig", 777);
        else if (isSXOS) {
            mkdir("sdmc:/sxos", 777);
            mkdir("sdmc:/sxos/exefs_patches", 777);
            mkdir("sdmc:/sxos/exefs_patches/ToCS3GraphicsConfig", 777);
        }
        return 0;
    }
    char MAGIC[6];
    fread(&MAGIC, 1, 5, config);
    if (strcmp(MAGIC, "IPS32") != 0) return 1;
    for (uint8_t i = 0; i <= 43; i++) {
        uint32_t m_offset = 0x0;
        uint8_t m_size = 0x0;
        fread(&m_offset, 4, 1, config);
        m_offset = __builtin_bswap32(m_offset);
        if (m_offset != pointers[i]) return i + 2;
        fread(&m_size, 1, 1, config);
        fread(&m_size, 1, 1, config);
        if ((m_size != 4) && (m_size != 8)) return i + 64;
        uint8_t m_assembler[m_size];
        fread(&m_assembler, 1, m_size, config);
        switch (m_offset) { //don't delete or change "else" lines, otherwise reading may not work for some reason my peasant mind can't understand
            case offset.aniso:
                if (memcmp(&m_assembler, &Instructions.aniso0, 4) == 0) anisoSelect = 0;
                else if (memcmp(&m_assembler, &Instructions.aniso2x, 4) == 0) anisoSelect = 1;
                else if (memcmp(&m_assembler, &Instructions.aniso4x, 4) == 0) anisoSelect = 2;
                else if (memcmp(&m_assembler, &Instructions.aniso8x, 4) == 0) anisoSelect = 3;
                else if (memcmp(&m_assembler, &Instructions.aniso16x, 4) == 0) anisoSelect = 4;
                else brls::Application::notify("AnisotropicFiltering Error"); 
				break;
            
            case offset.windowbuilderinterval:
                if (memcmp(&m_assembler, &Instructions.windowbuilderinterval2, 4) == 0) lock30FPSSelect = 1;
                else if (memcmp(&m_assembler, &Instructions.windowbuilderinterval1, 4) == 0) lock30FPSSelect = 0;
				else brls::Application::notify("30FPS Error");
                break;
            
            case offset.dockedshadowres:
                if (memcmp(&m_assembler, &Instructions.dockedshadowres512, 4) == 0) shadowRes_DockedSelect = 0;
                else if (memcmp(&m_assembler, &Instructions.dockedshadowres1024, 4) == 0) shadowRes_DockedSelect = 1;
                else if (memcmp(&m_assembler, &Instructions.dockedshadowres1536, 4) == 0) shadowRes_DockedSelect = 2;
                else if (memcmp(&m_assembler, &Instructions.dockedshadowres2048, 4) == 0) shadowRes_DockedSelect = 3;
                else if (memcmp(&m_assembler, &Instructions.dockedshadowres3072, 4) == 0) shadowRes_DockedSelect = 4;
                else if (memcmp(&m_assembler, &Instructions.dockedshadowres4096, 4) == 0) shadowRes_DockedSelect = 5;
				else brls::Application::notify("ShadowResDocked Error");
                break;
            
            case offset.handheldshadowres:
                if (memcmp(&m_assembler, &Instructions.handheldshadowres512, 4) == 0) shadowRes_HandheldSelect = 0;
                else if (memcmp(&m_assembler, &Instructions.handheldshadowres1024, 4) == 0) shadowRes_HandheldSelect = 1;
                else if (memcmp(&m_assembler, &Instructions.handheldshadowres1536, 4) == 0) shadowRes_HandheldSelect = 2;
                else if (memcmp(&m_assembler, &Instructions.handheldshadowres2048, 4) == 0) shadowRes_HandheldSelect = 3;
                else if (memcmp(&m_assembler, &Instructions.handheldshadowres3072, 4) == 0) shadowRes_HandheldSelect = 4;
                else if (memcmp(&m_assembler, &Instructions.handheldshadowres4096, 4) == 0) shadowRes_HandheldSelect = 5;
				else brls::Application::notify("ShadowResHandheld Error");
                break;
            
            case offset.dockedshadowdist:
                if (memcmp(&m_assembler, &Instructions.dockedshadowdist_01, 4) == 0) shadowDist_DockedSelect = 0;
                else if (memcmp(&m_assembler, &Instructions.dockedshadowdist_02, 4) == 0) shadowDist_DockedSelect = 1;
                else if (memcmp(&m_assembler, &Instructions.dockedshadowdist_03, 4) == 0) shadowDist_DockedSelect = 2;
                else if (memcmp(&m_assembler, &Instructions.dockedshadowdist_04, 4) == 0) shadowDist_DockedSelect = 3;
                else if (memcmp(&m_assembler, &Instructions.dockedshadowdist_05, 4) == 0) shadowDist_DockedSelect = 4;
                else if (memcmp(&m_assembler, &Instructions.dockedshadowdist_06, 4) == 0) shadowDist_DockedSelect = 5;
                else if (memcmp(&m_assembler, &Instructions.dockedshadowdist_07, 4) == 0) shadowDist_DockedSelect = 6;
                else if (memcmp(&m_assembler, &Instructions.dockedshadowdist_08, 4) == 0) shadowDist_DockedSelect = 7;
                else if (memcmp(&m_assembler, &Instructions.dockedshadowdist_09, 4) == 0) shadowDist_DockedSelect = 8;
                else if (memcmp(&m_assembler, &Instructions.dockedshadowdist_10, 4) == 0) shadowDist_DockedSelect = 9;
				else brls::Application::notify("ShadowDistDocked Error");
                break;

            case offset.handheldshadowdist:
                if (memcmp(&m_assembler, &Instructions.handheldshadowdist_01, 4) == 0) shadowDist_HandheldSelect = 0;
                else if (memcmp(&m_assembler, &Instructions.handheldshadowdist_02, 4) == 0) shadowDist_HandheldSelect = 1;
                else if (memcmp(&m_assembler, &Instructions.handheldshadowdist_03, 4) == 0) shadowDist_HandheldSelect = 2;
                else if (memcmp(&m_assembler, &Instructions.handheldshadowdist_04, 4) == 0) shadowDist_HandheldSelect = 3;
                else if (memcmp(&m_assembler, &Instructions.handheldshadowdist_05, 4) == 0) shadowDist_HandheldSelect = 4;
                else if (memcmp(&m_assembler, &Instructions.handheldshadowdist_06, 4) == 0) shadowDist_HandheldSelect = 5;
                else if (memcmp(&m_assembler, &Instructions.handheldshadowdist_07, 4) == 0) shadowDist_HandheldSelect = 6;
                else if (memcmp(&m_assembler, &Instructions.handheldshadowdist_08, 4) == 0) shadowDist_HandheldSelect = 7;
                else if (memcmp(&m_assembler, &Instructions.handheldshadowdist_09, 4) == 0) shadowDist_HandheldSelect = 8;
                else if (memcmp(&m_assembler, &Instructions.handheldshadowdist_10, 4) == 0) shadowDist_HandheldSelect = 9;
				else brls::Application::notify("ShadowDistHandheld Error");
                break;

            case offset.renderRes_W0:
                if (memcmp(&m_assembler, &Instructions.renderRes_WX_640, 4) == 0) renderingResSelect = 0;
				else if (memcmp(&m_assembler, &Instructions.renderRes_WX_800, 4) == 0) renderingResSelect = 1;
                else if (memcmp(&m_assembler, &Instructions.renderRes_WX_960, 4) == 0) renderingResSelect = 2;
				else if (memcmp(&m_assembler, &Instructions.renderRes_WX_1120, 4) == 0) renderingResSelect = 3;
                else if (memcmp(&m_assembler, &Instructions.renderRes_WX_1280, 4) == 0) renderingResSelect = 4;
                else if (memcmp(&m_assembler, &Instructions.renderRes_WX_1440, 4) == 0) renderingResSelect = 5;
                else if (memcmp(&m_assembler, &Instructions.renderRes_WX_1600, 4) == 0) renderingResSelect = 6;
				else if (memcmp(&m_assembler, &Instructions.renderRes_WX_1760, 4) == 0) renderingResSelect = 7;
                else if (memcmp(&m_assembler, &Instructions.renderRes_WX_1920, 4) == 0) renderingResSelect = 8;
				else brls::Application::notify("RendRes Error");
                break;

            case offset.windowRes_W0:
                if (memcmp(&m_assembler, &Instructions.windowRes_WX_1280, 4) == 0) windowResSelect = 0;
                else if (memcmp(&m_assembler, &Instructions.windowRes_WX_1920, 4) == 0) windowResSelect = 1;
				else brls::Application::notify("WindRes Error");
                break;
                
            default:
                break;
        }
    }
    fclose(config);
    return 0;
}

bool isServiceRunning(const char *serviceName) {	
    Handle handle;	
    SmServiceName service_name = smEncodeName(serviceName);	
    if (R_FAILED(smRegisterService(&handle, service_name, false, 1))) return true;
    else {
        svcCloseHandle(handle);	
        smUnregisterService(service_name);
        return false;
    }
}

uint8_t getAppIcon(uint64_t Tid, void* iconBufferPtr)
{	
    NsApplicationControlData appControlData;
    memset(&appControlData, 0x0, sizeof(NsApplicationControlData));
    size_t appControlDataSize = 0;
    if (R_FAILED(nsGetApplicationControlData(NsApplicationControlSource::NsApplicationControlSource_Storage, Tid, &appControlData, sizeof(NsApplicationControlData), &appControlDataSize))) {
        return 1;
    }
    memcpy(iconBufferPtr, appControlData.icon, sizeof(appControlData.icon));
    return 0;
}

typedef uint8_t NsApplicationIcon[0x20000];
NsApplicationIcon icon;

std::string getAppVersion(uint64_t Tid) {
    NsApplicationControlData appControlData;
    memset(&appControlData, 0x0, sizeof(NsApplicationControlData));

    size_t appControlDataSize = 0;
    if (R_FAILED(nsGetApplicationControlData(NsApplicationControlSource::NsApplicationControlSource_Storage, Tid, &appControlData, sizeof(NsApplicationControlData), &appControlDataSize)))
    {
        return std::string("1.0.0");
    }
    else return std::string(appControlData.nacp.display_version);
}

bool isGameInstalled(uint64_t Tid)
{
  NsApplicationRecord appRecords = {};
  int32_t actualAppRecordCnt = 0;
  Result rc;

  while (1)
  {
    static int32_t offset = 0;
    rc = nsListApplicationRecord(&appRecords, 1, offset, &actualAppRecordCnt);
    if (R_FAILED(rc) || (actualAppRecordCnt < 1) || appRecords.application_id == 0) break;
    if (appRecords.application_id == Tid) return true;
    offset++;
    appRecords = {};
  }
  return false;
}

int main(int argc, char *argv[])
{
    // Init the app

    if (!brls::Application::init("SaltyNX-Tool"))
    {
        brls::Logger::error("Unable to init SaltyNX-Tool");
        return EXIT_FAILURE;
    }
    
    i18n::loadTranslations();
    
    smInitialize();
    isAtmosphere = isServiceRunning("dmnt:cht");
    isSXOS = isServiceRunning("tx");
    smExit();
    
    
    if ((isAtmosphere ^ isSXOS) == 0) {
        brls::Application::crash("main/error/WrongCFW"_i18n);
        while (brls::Application::mainLoop());
        return EXIT_SUCCESS;
    }
	
	WritePaths();
    
    uint8_t error = ReadSettings();
    if (error != 0) {
        fclose(config);
        if (error == 1) brls::Application::notify("main/error/WrongMagic"_i18n);
        else if ((error <= 44) && (error >= 2)) {
            char Error_c[64] = "";
            sprintf(Error_c, "%s %u", "main/error/WrongOffset"_i18n.c_str(), error - 2);
            brls::Application::notify((std::string)Error_c);
        }
        else if ((error <= 128) && (error >=64)) {
            char Error_c[64] = "";
            sprintf(Error_c, "%s %u", "main/error/WrongSize"_i18n.c_str(), error - 64);
            brls::Application::notify((std::string)Error_c);
        }
		else if (error == 0xFF) brls::Application::notify("config not detected");
        else brls::Application::notify("main/error/WrongUnk"_i18n);
    }

    // Create a sample view
    brls::TabFrame* rootFrame = new brls::TabFrame();
    rootFrame->setTitle(APP_TITLE);
    nsInitialize();
	if (!isGameInstalled(0x01005420101DA000)) brls::Application::notify("main/error/NoGame"_i18n);
	else if (getAppVersion(0x01005420101DA000).compare("1.0.1") != 0) brls::Application::notify("main/error/WrongVersion"_i18n);
    if (getAppIcon(0x01005420101DA000, &icon) == 0) rootFrame->setIcon((unsigned char*)&icon, 0x20000);
    nsExit();
    
    brls::List* OptionsList = new brls::List();

    StatusItem = new brls::SelectListItem("main/settings/AnisotropicFiltering"_i18n, { "main/Disabled"_i18n, "2x", "4x", "8x", "16x"	}, anisoSelect, "main/settings/description/AnisotropicFiltering"_i18n);
    StatusItem->getValueSelectedEvent()->subscribe([](size_t selection) {
        anisoSelect = (uint8_t)selection;
    });
    StatusItem->setReduceDescriptionSpacing(true);
    
    OptionsList->addView(StatusItem);
    
    StatusItem2 = new brls::SelectListItem("main/settings/30FPS"_i18n, { "main/Disabled"_i18n, "main/Enabled"_i18n	}, lock30FPSSelect, "main/settings/description/30FPS"_i18n);
    StatusItem2->getValueSelectedEvent()->subscribe([](size_t selection) {
        lock30FPSSelect = (uint8_t)selection;
    });
    StatusItem2->setReduceDescriptionSpacing(true);
    
    OptionsList->addView(StatusItem2);
    
    StatusItem3 = new brls::SelectListItem("main/settings/ShadowResDocked"_i18n, { "512", "1024", "1536", "2048", "3072", "4096"	}, shadowRes_DockedSelect, "main/settings/description/ShadowResDocked"_i18n);
    StatusItem3->getValueSelectedEvent()->subscribe([](size_t selection) {
        shadowRes_DockedSelect = (uint8_t)selection;
    });
    StatusItem3->setReduceDescriptionSpacing(true);
    
    OptionsList->addView(StatusItem3);	
    
    StatusItem4 = new brls::SelectListItem("main/settings/ShadowResHandheld"_i18n, { "512", "1024", "1536", "2048", "3072", "4096"	}, shadowRes_HandheldSelect, "main/settings/description/ShadowResHandheld"_i18n);
    StatusItem4->getValueSelectedEvent()->subscribe([](size_t selection) {
        shadowRes_HandheldSelect = (uint8_t)selection;
    });
    StatusItem4->setReduceDescriptionSpacing(true);
    
    OptionsList->addView(StatusItem4);
    
    StatusItem5 = new brls::SelectListItem("main/settings/ShadowDistDocked"_i18n, { "0.125", "0.203125", "0.3125", "0.40625", "0.5", "0.59375", "0.6875", "0.8", "0.90625", "1.0"	}, shadowDist_DockedSelect, "main/settings/description/ShadowDistDocked"_i18n);
    StatusItem5->getValueSelectedEvent()->subscribe([](size_t selection) {
        shadowDist_DockedSelect = (uint8_t)selection;
    });
    StatusItem5->setReduceDescriptionSpacing(true);
    
    OptionsList->addView(StatusItem5);
    
    StatusItem6 = new brls::SelectListItem("main/settings/ShadowDistHandheld"_i18n, { "0.125", "0.203125", "0.3125", "0.40625", "0.5", "0.59375", "0.6875", "0.8", "0.90625", "1.0"	}, shadowDist_HandheldSelect, "main/settings/description/ShadowDistHandheld"_i18n);
    StatusItem6->getValueSelectedEvent()->subscribe([](size_t selection) {
        shadowDist_HandheldSelect = (uint8_t)selection;
    });
    StatusItem6->setReduceDescriptionSpacing(true);
    
    OptionsList->addView(StatusItem6);

    StatusItem8 = new brls::SelectListItem("main/settings/RendRes"_i18n, { "640x360", "800x450", "960x540", "1120x630", "1280x720", "1440x810", "1600x900", "1760x990", "1920x1080"	}, renderingResSelect, "main/settings/description/RendRes"_i18n);
    StatusItem8->getValueSelectedEvent()->subscribe([](size_t selection) {
        renderingResSelect = (uint8_t)selection;
    });
    StatusItem8->setReduceDescriptionSpacing(true);
    
    OptionsList->addView(StatusItem8);
    
    StatusItem9 = new brls::SelectListItem("main/settings/WindRes"_i18n, { "1280x720", "1920x1080"	}, windowResSelect, "main/settings/description/WindRes"_i18n);
    StatusItem9->getValueSelectedEvent()->subscribe([](size_t selection) {
        windowResSelect = (uint8_t)selection;
    });
    StatusItem9->setReduceDescriptionSpacing(true);
    
    OptionsList->addView(StatusItem9);
    
    OptionsList->registerAction("main/Save"_i18n, brls::Key::L, [] {
        if (SaveSettings() != 0) {
            fclose(config);
            brls::Application::notify("main/error/Save"_i18n);
        }
        else brls::Application::notify("main/Saved"_i18n);
        return true;
    });

    OptionsList->registerAction("main/Reset"_i18n, brls::Key::MINUS, [] {
        ResetToDefault();
        return true;
    });
    
    rootFrame->addTab("main/Options"_i18n, OptionsList);
    
    brls::List* ModList = new brls::List();
    
    brls::ListItem* ModItem = new brls::ListItem("main/mods/Vegetation"_i18n, "main/mods/description/Vegetation"_i18n);
    ModItem->getClickEvent()->subscribe([](brls::View* view) {
        if (isVegetationMod()) {
            brls::Dialog* dialog = new brls::Dialog("main/mods/Warning"_i18n);
            brls::GenericEvent::Callback Accept = [dialog](brls::View* view) {
                dialog->close();
                threadWaitForExit(&t0);
                threadClose(&t0);
                brls::Application::notify("main/mods/notify/VegetationRemoveStart"_i18n);
                threadCreate(&t0, removeVegetationMod, NULL, NULL, 0x100000, 0x28, 0);
                threadStart(&t0);
            };
            
            dialog->addButton("main/Continue"_i18n, Accept);
            
            dialog->open();
        }
        else {
            threadWaitForExit(&t0);
            threadClose(&t0);
            brls::Application::notify("main/mods/notify/VegetationStart"_i18n);
            threadCreate(&t0, InstallVegetationMod, NULL, NULL, 0x100000, 0x28, 0);
            threadStart(&t0);
        }
        return true;
        
    });
    ModItem->setReduceDescriptionSpacing(true);
    
    ModList->addView(ModItem);
    
    rootFrame->addTab("main/Mod"_i18n, ModList);
    
    rootFrame->addTab("main/About"_i18n, new AboutTab());


    // Add the root view to the stack
    brls::Application::pushView(rootFrame);

    // Run the app
    while (brls::Application::mainLoop());

    // Exit
    threadWaitForExit(&t0);
    threadClose(&t0);
    return EXIT_SUCCESS;
}

