
//Declaration of leaves types
   Int_t           trig;
   Int_t           kType;
   Int_t           Run;
   Int_t           Event;
   Int_t           evType;
   Int_t           L0TisTos;
   Int_t           HltTisTos;
   Float_t         kx;
   Float_t         ky;
   Float_t         kz;
   Int_t           Tag;
   Float_t         Omega;
   Int_t           TagCat;
   Int_t           TagOS;
   Float_t         OmegaOS;
   Int_t           TagCatOS;
   Int_t           TrueTag;
   Int_t           bkgCat;
   Int_t           BSosc;
   Float_t         BSx;
   Float_t         BSy;
   Float_t         BSz;
   Int_t           BOID;
   Float_t         BOP;
   Float_t         BOthe;
   Float_t         BOphi;
   Float_t         BOx;
   Float_t         BOy;
   Float_t         BOz;
   Int_t           BOosc;
   Int_t           krec;
   Float_t         RVx;
   Float_t         RVy;
   Float_t         RVz;
   Float_t         RVx_r;
   Float_t         RVy_r;
   Float_t         RVz_r;
   Float_t         tau;
   Float_t         tauErr;
   Float_t         ctChi2;
   Int_t           M;
   Float_t         sID[10];
   Float_t         sMothID[10];
   Float_t         sP[10];
   Float_t         sPt[10];
   Float_t         sPhi[10];
   Float_t         sMass[10];
   Float_t         sMCID[10];
   Float_t         sMCMothID[10];
   Float_t         sMCP[10];
   Float_t         sMCPt[10];
   Float_t         sMCPhi[10];
   Float_t         sVx[10];
   Float_t         sVy[10];
   Float_t         sVz[10];
   Int_t           T;
   Float_t         TaggerType[10];
   Float_t         TaggerDecision[10];
   Float_t         TaggerOmega[10];
   Int_t           N;
   Float_t         ID[200];
   Float_t         P[200];
   Float_t         Pt[200];
   Float_t         phi[200];
   Float_t         ch[200];
   Float_t         ip[200];
   Float_t         iperr[200];
   Float_t         ip_r[200];
   Float_t         iperr_r[200];
   Float_t         ipPU[200];
   Float_t         trtyp[200];
   Float_t         lcs[200];
   Float_t         tsal[200];
   Float_t         distPhi[200];
   Float_t         veloch[200];
   Float_t         EOverP[200];
   Float_t         PIDe[200];
   Float_t         PIDm[200];
   Float_t         PIDk[200];
   Float_t         PIDp[200];
   Float_t         PIDfl[200];
   Float_t         MCID[200];
   Float_t         MCP[200];
   Float_t         MCPt[200];
   Float_t         MCphi[200];
   Float_t         MCz[200];
   Float_t         mothID[200];
   Float_t         ancID[200];
   Float_t         bFlag[200];
   Float_t         xFlag[200];
   Float_t         vFlag[200];

   // Set branch addresses.
   mytagging->SetBranchAddress("trig",&trig);
   mytagging->SetBranchAddress("kType",&kType);
   mytagging->SetBranchAddress("Run",&Run);
   mytagging->SetBranchAddress("Event",&Event);
   mytagging->SetBranchAddress("evType",&evType);
   mytagging->SetBranchAddress("L0TisTos",&L0TisTos);
   mytagging->SetBranchAddress("HltTisTos",&HltTisTos);
   mytagging->SetBranchAddress("kx",&kx);
   mytagging->SetBranchAddress("ky",&ky);
   mytagging->SetBranchAddress("kz",&kz);
   mytagging->SetBranchAddress("Tag",&Tag);
   mytagging->SetBranchAddress("Omega",&Omega);
   mytagging->SetBranchAddress("TagCat",&TagCat);
   mytagging->SetBranchAddress("TagOS",&TagOS);
   mytagging->SetBranchAddress("OmegaOS",&OmegaOS);
   mytagging->SetBranchAddress("TagCatOS",&TagCatOS);
   mytagging->SetBranchAddress("TrueTag",&TrueTag);
   mytagging->SetBranchAddress("bkgCat",&bkgCat);
   mytagging->SetBranchAddress("BSosc",&BSosc);
   mytagging->SetBranchAddress("BSx",&BSx);
   mytagging->SetBranchAddress("BSy",&BSy);
   mytagging->SetBranchAddress("BSz",&BSz);
   mytagging->SetBranchAddress("BOID",&BOID);
   mytagging->SetBranchAddress("BOP",&BOP);
   mytagging->SetBranchAddress("BOthe",&BOthe);
   mytagging->SetBranchAddress("BOphi",&BOphi);
   mytagging->SetBranchAddress("BOx",&BOx);
   mytagging->SetBranchAddress("BOy",&BOy);
   mytagging->SetBranchAddress("BOz",&BOz);
   mytagging->SetBranchAddress("BOosc",&BOosc);
   mytagging->SetBranchAddress("krec",&krec);
   mytagging->SetBranchAddress("RVx",&RVx);
   mytagging->SetBranchAddress("RVy",&RVy);
   mytagging->SetBranchAddress("RVz",&RVz);
   mytagging->SetBranchAddress("RVx_r",&RVx_r);
   mytagging->SetBranchAddress("RVy_r",&RVy_r);
   mytagging->SetBranchAddress("RVz_r",&RVz_r);
   mytagging->SetBranchAddress("tau",&tau);
   mytagging->SetBranchAddress("tauErr",&tauErr);
   mytagging->SetBranchAddress("ctChi2",&ctChi2);
   mytagging->SetBranchAddress("M",&M);
   mytagging->SetBranchAddress("sID",sID);
   mytagging->SetBranchAddress("sMothID",sMothID);
   mytagging->SetBranchAddress("sP",sP);
   mytagging->SetBranchAddress("sPt",sPt);
   mytagging->SetBranchAddress("sPhi",sPhi);
   mytagging->SetBranchAddress("sMass",sMass);
   mytagging->SetBranchAddress("sMCID",sMCID);
   mytagging->SetBranchAddress("sMCMothID",sMCMothID);
   mytagging->SetBranchAddress("sMCP",sMCP);
   mytagging->SetBranchAddress("sMCPt",sMCPt);
   mytagging->SetBranchAddress("sMCPhi",sMCPhi);
   mytagging->SetBranchAddress("sVx",sVx);
   mytagging->SetBranchAddress("sVy",sVy);
   mytagging->SetBranchAddress("sVz",sVz);
   mytagging->SetBranchAddress("T",&T);
   mytagging->SetBranchAddress("TaggerType",TaggerType);
   mytagging->SetBranchAddress("TaggerDecision",TaggerDecision);
   mytagging->SetBranchAddress("TaggerOmega",TaggerOmega);
   mytagging->SetBranchAddress("N",&N);
   mytagging->SetBranchAddress("ID",ID);
   mytagging->SetBranchAddress("P",P);
   mytagging->SetBranchAddress("Pt",Pt);
   mytagging->SetBranchAddress("phi",phi);
   mytagging->SetBranchAddress("ch",ch);
   mytagging->SetBranchAddress("ip",ip);
   mytagging->SetBranchAddress("iperr",iperr);
   mytagging->SetBranchAddress("ip_r",ip_r);
   mytagging->SetBranchAddress("iperr_r",iperr_r);
   mytagging->SetBranchAddress("ipPU",ipPU);
   mytagging->SetBranchAddress("trtyp",trtyp);
   mytagging->SetBranchAddress("lcs",lcs);
   mytagging->SetBranchAddress("tsal",tsal);
   mytagging->SetBranchAddress("distPhi",distPhi);
   mytagging->SetBranchAddress("veloch",veloch);
   mytagging->SetBranchAddress("EOverP",EOverP);
   mytagging->SetBranchAddress("PIDe",PIDe);
   mytagging->SetBranchAddress("PIDm",PIDm);
   mytagging->SetBranchAddress("PIDk",PIDk);
   mytagging->SetBranchAddress("PIDp",PIDp);
   mytagging->SetBranchAddress("PIDfl",PIDfl);
   mytagging->SetBranchAddress("MCID",MCID);
   mytagging->SetBranchAddress("MCP",MCP);
   mytagging->SetBranchAddress("MCPt",MCPt);
   mytagging->SetBranchAddress("MCphi",MCphi);
   mytagging->SetBranchAddress("MCz",MCz);
   mytagging->SetBranchAddress("mothID",mothID);
   mytagging->SetBranchAddress("ancID",ancID);
   mytagging->SetBranchAddress("bFlag",bFlag);
   mytagging->SetBranchAddress("xFlag",xFlag);
   mytagging->SetBranchAddress("vFlag",vFlag);
