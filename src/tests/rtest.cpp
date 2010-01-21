#include <string>
#include <iostream>
#include "Ptexture.h"
#include <cstdlib>
#include <cstdio> // printf()

void DumpData(Ptex::DataType dt, int nchan, PtexFaceData* dh, std::string prefix)
{
    void* dpixel = malloc(Ptex::DataSize(dt)*nchan);
    float* pixel = (float*) malloc(sizeof(float)*nchan);
    uint8_t* cpixel = (uint8_t*) malloc(sizeof(uint8_t)*nchan);
    Ptex::Res res = dh->res();
    printf("%sdata (%d x %d)", prefix.c_str(), res.u(), res.v());
    if (dh->isTiled()) {
	Ptex::Res tileres = dh->tileRes();
	printf(", tiled (%d x %d):\n", tileres.u(), tileres.v());
	int n = res.ntiles(tileres);
	for (int i = 0; i < n; i++) {
	    PtexFaceData* t = dh->getTile(i);
	    std::cout << prefix << "  tile " << i;
	    if (!t) {
		std::cout << " NULL!" << std::endl;
	    } else {
		DumpData(dt, nchan, t, prefix + "  ");
		t->release();
	    }
	}
    } else {
	int ures, vres;
	if (dh->isConstant()) { ures = vres = 1; std::cout << ", const: "; }
	else { ures = res.u(); vres = res.v(); std::cout << ":\n"; }
	
	int vimax = vres;// if (vimax > 16) vimax = 16;
	for (int vi = 0; vi < vimax; vi++) {
	    if (vi == 8 && vres > 16) { vi = vres-8; std::cout << prefix << "  ..." << std::endl; }
	    std::cout << prefix << "  ";
	    int uimax = ures;// if (uimax > 16) uimax = 16;
	    for (int ui = 0; ui < uimax; ui++) {
		if (ui == 8 && ures > 16) { ui = ures-8; std::cout << "... "; }
		dh->getPixel(ui, vi, dpixel);
		Ptex::ConvertToFloat(pixel, dpixel, dt, nchan);
		Ptex::ConvertFromFloat(cpixel, pixel, Ptex::dt_uint8, nchan);
		for (int c=0; c < nchan; c++) {
		    printf("%02x", cpixel[c]);
		}
		printf(" ");
	    }
	    if (uimax != ures) printf(" ...");
	    printf("\n");
	}
	if (vimax != vres) std::cout << prefix << "  ..." << std::endl;
    }
    free(cpixel);
    free(pixel);
    free(dpixel);
}

template <typename T>
class DumpMetaArrayVal
{
 public:
    void operator()(PtexMetaData* meta, const char* key)
    {
	const T* val=0;
	int count=0;
	meta->getValue(key, val, count);
	for (int i = 0; i < count; i++) {
	    if (i%10==0 && (i || count > 10)) std::cout << "\n  ";
	    std::cout <<  "  " << val[i];
	}
    }
};


void DumpMetaData(PtexMetaData* meta)
{
    std::cout << "meta:" << std::endl;
    for (int i = 0; i < meta->numKeys(); i++) {
	const char* key;
	Ptex::MetaDataType type;
	meta->getKey(i, key, type);
	std::cout << "  " << key << " type=" << Ptex::MetaDataTypeName(type);
	switch (type) {
	case Ptex::mdt_string:
	    {
		const char* val=0;
		meta->getValue(key, val);
		std::cout <<  "  " << val;
	    }
	    break;
	case Ptex::mdt_int8:   DumpMetaArrayVal<int8_t>()(meta, key); break;
	case Ptex::mdt_int16:  DumpMetaArrayVal<int16_t>()(meta, key); break;
	case Ptex::mdt_int32:  DumpMetaArrayVal<int32_t>()(meta, key); break;
	case Ptex::mdt_float:  DumpMetaArrayVal<float>()(meta, key); break;
	case Ptex::mdt_double: DumpMetaArrayVal<double>()(meta, key); break;
	}
	std::cout << std::endl;
    }
}

int main(int /*argc*/, char** /*argv*/)
{
    Ptex::String error;
    PtexTexture* r = PtexTexture::open("test.ptx", error);

    if (!r) {
	std::cerr << error.c_str() << std::endl;
	return 1;
    }
    std::cout << "meshType: " << Ptex::MeshTypeName(r->meshType()) << std::endl;
    std::cout << "dataType: " << Ptex::DataTypeName(r->dataType()) << std::endl;
    std::cout << "numChannels: " << r->numChannels() << std::endl;
    std::cout << "alphaChannel: ";
    if (r->alphaChannel() == -1) std::cout << "(none)" << std::endl;
    else std::cout << r->alphaChannel() << std::endl;
    std::cout << "numFaces: " << r->numFaces() << std::endl;

    PtexMetaData* meta = r->getMetaData();
    std::cout << "numMetaKeys: " << meta->numKeys() << std::endl;
    if (meta->numKeys()) DumpMetaData(meta);
    meta->release();

    int nfaces = r->numFaces();
    for (int i = 0; i < nfaces; i++) {
	const Ptex::FaceInfo& f = r->getFaceInfo(i);
	std::cout << "face " << i << ":\n"
		  << "  res: " << int(f.res.ulog2) << ' ' << int(f.res.vlog2) << "\n"
		  << "  adjface: " 
		  << f.adjfaces[0] << ' '
		  << f.adjfaces[1] << ' '
		  << f.adjfaces[2] << ' '
		  << f.adjfaces[3] << "\n"
		  << "  adjedge: " 
		  << f.adjedge(0) << ' '
		  << f.adjedge(1) << ' '
		  << f.adjedge(2) << ' '
		  << f.adjedge(3) << "\n"
		  << "  flags: " << int(f.flags) << "\n";

	Ptex::Res res = f.res;
	while (res.ulog2 > 0 || res.vlog2 > 0) {
	    PtexFaceData* dh = r->getData(i, res);
	    if (!dh) break;
	    DumpData(r->dataType(), r->numChannels(), dh, "  ");
	    bool isconst = dh->isConstant();
	    dh->release();
	    if (isconst) break;
	    if (res.ulog2) res.ulog2--;
	    if (res.vlog2) res.vlog2--;
	}
	PtexFaceData* dh = r->getData(i, Ptex::Res(0,0));
	DumpData(r->dataType(), r->numChannels(), dh, "  ");
	dh->release();
    }

    r->release();
    return 0;
}
