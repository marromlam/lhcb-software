drop table CONDITIONS;
drop table ANAMESSAGE;
drop table SHOWHISTO;
drop table PAGE;
drop table PAGEFOLDER;
drop table ANASETTINGS;
drop table ANALYSIS;
drop table HCREATOR;
drop table ALGORITHM;
drop table DIMSERVICENAME;
drop table HISTOGRAM;
drop table HISTOGRAMSET;
drop table DISPLAYOPTIONS;
drop table FITFUNCTION;
drop table TASK;
drop table SUBSYSTEM;
drop table ERGOSUM;

drop type DISPOPT;

drop sequence HistogramSet_ID;
drop sequence Analysis_ID;
drop sequence Displayoptions_ID;
drop sequence AnaMessage_ID;
drop sequence FunCode_ID;
drop sequence SHH_ID;

purge recyclebin;
CREATE OR REPLACE TYPE vthresholds as VARRAY(50) OF FLOAT;
/
CREATE OR REPLACE TYPE vparameters as VARRAY(50) OF VARCHAR2(15);
/
CREATE OR REPLACE TYPE labels as VARRAY(500) OF varchar2(30);
/
CREATE OR REPLACE TYPE intlist as VARRAY(20) OF int;
/
CREATE OR REPLACE TYPE analist is VARRAY(20) OF varchar2(30);
/
CREATE OR REPLACE TYPE flolist is VARRAY(20) OF float;
/
CREATE OR REPLACE TYPE hnalist is VARRAY(100) OF varchar2(130);
/
CREATE OR REPLACE TYPE inttlist is TABLE OF int;
/
CREATE OR REPLACE TYPE DISPOPT AS OBJECT (
  LABEL_X varchar2(50),
  LABEL_Y varchar2(50),
  LABEL_Z varchar2(50),
  YMIN float,
  YMAX float,
  STATS smallint,
  FILLSTYLE smallint,
  FILLCOLOR smallint,
  LINESTYLE smallint,
  LINECOLOR smallint,
  LINEWIDTH smallint,
  DRAWOPTS varchar2(50),
  XMIN         FLOAT,
  XMAX         FLOAT,
  ZMIN         FLOAT,
  ZMAX         FLOAT,
  LOGX         NUMBER(1),
  LOGY         NUMBER(1),
  LOGZ         NUMBER(1),
  TIMAGE       VARCHAR2(5),
  REF          VARCHAR2(5),
  REFRESH      FLOAT,
  TIT_X_SIZE   FLOAT,
  TIT_X_OFFS   FLOAT,
  TIT_Y_SIZE   FLOAT,
  TIT_Y_OFFS   FLOAT,
  TIT_Z_SIZE   FLOAT,
  TIT_Z_OFFS   FLOAT,
  LAB_X_SIZE   FLOAT,
  LAB_X_OFFS   FLOAT,
  LAB_Y_SIZE   FLOAT,
  LAB_Y_OFFS   FLOAT,
  LAB_Z_SIZE   FLOAT,
  LAB_Z_OFFS   FLOAT,
  GRIDX        NUMBER(1),
  GRIDY        NUMBER(1),
  THETA        FLOAT,
  PHI          FLOAT,
  CNTPLOT      VARCHAR2(5),
  DRAWPATTERN varchar2(100),
  STAT_X_SIZE   FLOAT,
  STAT_X_OFFS   FLOAT,
  STAT_Y_SIZE   FLOAT,
  STAT_Y_OFFS   FLOAT,
  HTIT_X_SIZE   FLOAT,
  HTIT_X_OFFS   FLOAT,
  HTIT_Y_SIZE   FLOAT,
  HTIT_Y_OFFS   FLOAT,
  NDIVX         smallint,
  NDIVY         smallint,
  MARKERSIZE    FLOAT,
  MARKERCOLOR   smallint,
  MARKERSTYLE   smallint,
  NORM          FLOAT,
  TICK_X        smallint,
  TICK_Y        smallint,
  MARGIN_TOP    FLOAT,
  MARGIN_BOTTOM FLOAT,
  MARGIN_LEFT   FLOAT,
  MARGIN_RIGHT  FLOAT,
  PADCOLOR      smallint,
  STATTRANSP    NUMBER(1),
  REFDRAWOPTS varchar2(50),
  SPAREI1     smallint,
  SPAREI2     smallint,
  SPAREF1     FLOAT,
  SPAREF2     FLOAT,
  SPARES     varchar2(50),
  NOTITLE     NUMBER(1),
  SHOWTITLE  varchar2(80)
);
/

create table ERGOSUM (
 version number(5),
 apiversion varchar2(10),
 alglist number(8)
);

create table SUBSYSTEM (
 SSName varchar2(10)  constraint SS_pk primary key
	USING INDEX (create index SS_pk_ix on SUBSYSTEM(SSName) )
);


create table TASK (
 TASKNAME varchar2(64)  constraint TK_PK primary key 
	USING INDEX (create index TK_PK_IX on Task(TaskName) ), 
 RunOnPhysics number(1),
 RunOnCalib number(1),
 RunOnEmpty number(1),
 SUBSYS1 VARCHAR2(10) CONSTRAINT TK_SS1 references SUBSYSTEM(SSName) ON DELETE SET NULL,
 SUBSYS2 VARCHAR2(10) CONSTRAINT TK_SS2 references SUBSYSTEM(SSName) ON DELETE SET NULL,
 SUBSYS3 VARCHAR2(10) CONSTRAINT TK_SS3 references SUBSYSTEM(SSName) ON DELETE SET NULL,
 SaveFrequency float,
 REFERENCE  varchar2(100)
);

CREATE INDEX TK_SS1_IX on TASK(SUBSYS1) ;
CREATE INDEX TK_SS2_IX on TASK(SUBSYS2) ;
CREATE INDEX TK_SS3_IX on TASK(SUBSYS3) ;

CREATE SEQUENCE FunCode_ID START WITH 1
      NOMAXVALUE
      NOCACHE; 

create table FITFUNCTION (
 NAME varchar2(15)  constraint FIT_PK primary key 
	USING INDEX (create index FIT_PK_IX on FITFUNCTION(NAME) ), 
 CODE int constraint nnu_code NOT NULL,
 DOC varchar2(1000),
 NP smallint constraint nnu_np  NOT NULL,
 VPARNAMES vparameters DEFAULT NULL,
 MUSTINIT number(1) DEFAULT 0 constraint  nnu_mustinit NOT NULL,
 NINPUT smallint DEFAULT 0,
 VFIPARDEFVAL vthresholds DEFAULT NULL
);

CREATE SEQUENCE Displayoptions_ID START WITH 1
      NOMAXVALUE
      NOCACHE; 

create table DISPLAYOPTIONS (
  DOID integer constraint DO_PK primary key 
        USING INDEX (create index DO_PK_IX on DISPLAYOPTIONS(DOID) ),
  OPT dispopt,
  FITFUN varchar2(15) CONSTRAINT DO_FITF references FITFUNCTION(NAME) ON DELETE SET NULL,
  VFITPARS vthresholds DEFAULT NULL,
  VFITRANGE vthresholds DEFAULT NULL
);
CREATE INDEX DO_FITF_IX on DISPLAYOPTIONS(FITFUN);


CREATE SEQUENCE HistogramSet_ID START WITH 1
      NOMAXVALUE
      CACHE 50;   


create table HISTOGRAMSET (
 HSID integer  constraint HS_PK primary key 
	USING INDEX (create index HS_PK_IX on HISTOGRAMSET(HSID) ),
 NHS number(6) DEFAULT 1 constraint  nnu_nhs NOT NULL ,
 HSTASK varchar2(64)  constraint nnu_hstask NOT NULL CONSTRAINT HS_TK references TASK(TASKNAME),
 HSALGO varchar2(64) constraint nnu_hsalgo NOT NULL,
 HSTITLE varchar2(100) constraint nnu_hstitle NOT NULL,
 CONSTRAINT HS_UNQ UNIQUE (HSTASK,HSALGO,HSTITLE) USING INDEX ,
 HSTYPE varchar2(3) constraint  nnu_hstype NOT NULL CONSTRAINT HS_TY_CK CHECK (HSTYPE IN ('H1D','H2D','P1D','P2D','CNT','TRE')),
 NANALYSIS number(2) DEFAULT 0,
 DESCR varchar2(4000),
 DOC varchar2(200),
 HSDISPLAY integer  CONSTRAINT HS_DISP references DISPLAYOPTIONS(DOID) ON DELETE SET NULL
);
CREATE INDEX HS_TIT_IX ON HISTOGRAMSET(HSTITLE) ;
CREATE INDEX HS_ALG_IX ON HISTOGRAMSET(HSALGO) ;
CREATE INDEX HS_TK_IX on HISTOGRAMSET(HSTASK) ;



CREATE INDEX HS_DISP_IX on HISTOGRAMSET(HSDISPLAY) ;


create table HISTOGRAM (
 HID varchar2(12)  constraint H_PK primary key 
	USING INDEX (create index H_PK_IX ON HISTOGRAM(HID) ),
 NAME varchar2(130)  constraint  nnu_name NOT NULL,
 HSET integer constraint  nnu_hset NOT NULL CONSTRAINT H_SET references HISTOGRAMSET(HSID) ON DELETE CASCADE,
 IHS number(6) DEFAULT 1 constraint nnu_ihs NOT NULL ,
 SUBTITLE varchar2(50),
 CONSTRAINT H_UNQ UNIQUE (HSET,SUBTITLE) USING INDEX,
 IsTest   number(1),
 IsAnalysisHist number(1) DEFAULT 0 constraint nnu_isanalysishist NOT NULL,
 CREATION TIMESTAMP,
 OBSOLETENESS TIMESTAMP,
 DISPLAY integer  CONSTRAINT H_DISP references DISPLAYOPTIONS(DOID) ON DELETE SET NULL,
 VBINLABELS labels,
 NBINLABX smallint,
 NBINLABY smallint,
 REFPAGE  varchar2(350) DEFAULT NULL
);
CREATE INDEX H_SET_IX on HISTOGRAM(HSET) ;
CREATE INDEX H_SUB_IX on HISTOGRAM(SUBTITLE) ;
CREATE INDEX H_DISP_IX on HISTOGRAM(DISPLAY) ;
CREATE INDEX H_NAME_IX on HISTOGRAM(NAME) ;

create table DIMSERVICENAME (
 SN varchar2(130) constraint DSN_PK primary key 
	USING INDEX (create index DSN_PK_IX on DIMSERVICENAME(SN) ),
 PUBHISTO varchar2(12) constraint DSN_PH references HISTOGRAM(HID) ON DELETE CASCADE
);
CREATE INDEX DSN_PHIX on DIMSERVICENAME(PUBHISTO) ;



create table ALGORITHM (
 ALGNAME varchar2(30) constraint AL_PK primary key 
	USING INDEX (create index AL_PK_IX ON ALGORITHM(ALGNAME) ),
 ALGTYPE varchar2(8)  constraint  nnu_algtype NOT NULL CONSTRAINT AL_TY_CK CHECK ( ALGTYPE IN ('HCREATOR','CHECK')),
 NINPUT smallint DEFAULT 0,
 GETSET number(1) DEFAULT 0,
 NPARS smallint DEFAULT 0,
 VALGPARS vparameters default NULL,
 VPARDEFVAL vthresholds default NULL,
 ALGDOC varchar2(1000),
 HCTYPE varchar2(3) default NULL
);


create table HCREATOR (
 HCID varchar2(12)  constraint nnu_hcid NOT NULL constraint HC_ID references HISTOGRAM(HID) ON DELETE CASCADE,
 ALGORITHM varchar2(30) constraint  nnu_algorithm NOT NULL constraint HC_ALGNAME references ALGORITHM(ALGNAME),
 SOURCEH1 varchar2(12) CONSTRAINT HC_SH1 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCEH2 varchar2(12) CONSTRAINT HC_SH2 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCEH3 varchar2(12) CONSTRAINT HC_SH3 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCEH4 varchar2(12) CONSTRAINT HC_SH4 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCEH5 varchar2(12) CONSTRAINT HC_SH5 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCEH6 varchar2(12) CONSTRAINT HC_SH6 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCEH7 varchar2(12) CONSTRAINT HC_SH7 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCEH8 varchar2(12) CONSTRAINT HC_SH8 references HISTOGRAM(HID) ON DELETE CASCADE,
 SOURCESET integer CONSTRAINT HC_SSET references HISTOGRAMSET(HSID) ON DELETE CASCADE,
 VHCPARS vthresholds default NULL
);

CREATE INDEX HC_ID_IX on HCREATOR(HCID) ;
CREATE INDEX HC_AL_IX on HCREATOR(ALGORITHM) ;
CREATE INDEX HC_SH1_IX on HCREATOR(SOURCEH1) ;
CREATE INDEX HC_SH2_IX on HCREATOR(SOURCEH2) ;
CREATE INDEX HC_SH3_IX on HCREATOR(SOURCEH3) ;
CREATE INDEX HC_SH4_IX on HCREATOR(SOURCEH4) ;
CREATE INDEX HC_SH5_IX on HCREATOR(SOURCEH5) ;
CREATE INDEX HC_SH6_IX on HCREATOR(SOURCEH6) ;
CREATE INDEX HC_SH7_IX on HCREATOR(SOURCEH7) ;
CREATE INDEX HC_SH8_IX on HCREATOR(SOURCEH8) ;
CREATE INDEX HC_SSET_IX on HCREATOR(SOURCESET) ;

CREATE SEQUENCE Analysis_ID START WITH 1
      NOMAXVALUE
      NOCACHE; 

create table ANALYSIS (
 AID integer  constraint A_PK primary key 
	USING INDEX (create index A_PK_IX ON ANALYSIS(AID) ),
 HSET integer constraint nnu_anahset NOT NULL CONSTRAINT A_HSET references HISTOGRAMSET(HSID) ON DELETE CASCADE,
 ALGORITHM varchar2(30) constraint nnu_anaalgorithm NOT NULL CONSTRAINT A_ALGNAME references ALGORITHM(ALGNAME),
 ANADOC varchar2(2000),
 ANAMESSAGE varchar2(200),
 MINSTATS int
);
CREATE INDEX A_HSET_IX on ANALYSIS(HSET) ;
CREATE INDEX A_AL_IX on ANALYSIS(ALGORITHM) ;


create table ANASETTINGS (
 ANA integer constraint nnu_ana NOT NULL CONSTRAINT ASET_ANA references ANALYSIS(AID) ON DELETE CASCADE,
 HISTO  varchar2(12) constraint nnu_anahisto NOT NULL CONSTRAINT ASET_HISTO references HISTOGRAM(HID) ON DELETE CASCADE,
 CONSTRAINT ASET_UNQ UNIQUE (ANA,HISTO) USING INDEX ,
 MASK number(1) DEFAULT 0 constraint  nnu_mask NOT NULL ,
 VWARNINGS vthresholds,
 VALARMS vthresholds,
 VINPUTPARS vthresholds,
 STATUSBITS smallint,
 MINBINSTAT FLOAT,
 MINBINSTATFRAC FLOAT
);
CREATE INDEX ASET_ANA_IX on ANASETTINGS(ANA) ;
CREATE INDEX ASET_HISTO_IX on ANASETTINGS(HISTO) ;

-- for backw. comp.
CREATE or replace FUNCTION PAGEFULLNAME(name VARCHAR2, folder VARCHAR2) RETURN VARCHAR2 AS
BEGIN
   return name;
END;
/

create table PAGEFOLDER (
 PageFolderName varchar2(300) constraint PGF_pk primary key
	USING INDEX (create index PGF_pk_ix on PAGEFOLDER(PageFolderName) ),
 Parent varchar2(300) default NULL 
);

create table PAGE (
 PageName varchar2(350)  constraint PG_pk primary key
       USING INDEX (create index PG_pk_ix on PAGE(PageName) ),
 Folder varchar2(300) constraint nnu_folder NOT NULL CONSTRAINT PG_FD references PAGEFOLDER(PageFolderName),
 NHisto integer,
 PageDoc  varchar2(2000),
 PAGEPATTERN varchar2(100)
);
CREATE INDEX PG_FD_IX on PAGE(Folder) ; 

CREATE SEQUENCE SHH_ID START WITH 1
      NOMAXVALUE
      NOCACHE; 

create table SHOWHISTO (
 SHID int constraint SHH_pk  primary key USING INDEX (create index SHH_pk_ix on SHOWHISTO(SHID)),
 PAGE varchar2(350) constraint  nnu_page NOT NULL CONSTRAINT SHH_PAGE references PAGE(PageName) ON DELETE CASCADE,
 PAGEFOLDER varchar2(300) constraint nnu_pagefolder NOT NULL,
 HISTO  varchar2(12) constraint nnu_showhistohisto NOT NULL CONSTRAINT SHH_HISTO references HISTOGRAM(HID),
 INSTANCE smallint DEFAULT 1 constraint nnu_instance NOT NULL,
 CONSTRAINT SHH_UNQ UNIQUE (PAGE,HISTO,INSTANCE) USING INDEX ,
 CENTER_X real constraint  nnu_centerx NOT NULL,
 CENTER_Y real constraint nnu_centery NOT NULL,
 SIZE_X real constraint nnu_sizex NOT NULL,
 SIZE_Y real constraint nnu_sizey NOT NULL,
 MOTHERH int CONSTRAINT SHH_MOTH references SHOWHISTO(SHID) ON DELETE SET NULL,
 IOVERLAP smallint,
 SDISPLAY integer  CONSTRAINT SH_DISP references DISPLAYOPTIONS(DOID) ON DELETE SET NULL
);
CREATE INDEX SHH_PAGE_IX on SHOWHISTO(PAGE) ;
CREATE INDEX SHH_HISTO_IX on SHOWHISTO(HISTO) ;

CREATE SEQUENCE AnaMessage_ID START WITH 1
      NOMAXVALUE
      NOCACHE; 

create table ANAMESSAGE (
 ID int constraint ALR_pk primary key
       USING INDEX (create index ALR_pk_ix on ANAMESSAGE(ID) ),
 HISTO varchar2(12) CONSTRAINT ALR_HISTO references HISTOGRAM(HID) ON DELETE CASCADE,
 SAVESET varchar2(500) constraint nnu_saveset NOT NULL,
 TASK varchar2(64) CONSTRAINT ALR_TASK references TASK(TASKNAME), 
 ANALYSISTASK varchar2(64),
 ALEVEL varchar2(7)  constraint nnu_alevel NOT NULL CONSTRAINT ALR_LE_CK CHECK (ALEVEL IN ('INFO','WARNING','ALARM')) ,
 MSGTEXT varchar2(4000),
 ANAID integer CONSTRAINT ALR_AID references ANALYSIS(AID) ON DELETE CASCADE,
 ANANAME varchar2(300),
 MSGTIME TIMESTAMP,
 MSGLASTTIME TIMESTAMP,
 ACTIVE number(1) DEFAULT 1 constraint  nnu_active NOT NULL,
 NOCCUR smallint,
 NSOLV  smallint,
 NRETRIG smallint
);
CREATE INDEX ALR_HIIX on ANAMESSAGE(HISTO);
CREATE INDEX ALR_ANATASK on ANAMESSAGE(ANALYSISTASK);


create table CONDITIONS (
 IBIT smallint constraint COND_pk primary key
       USING INDEX (create index COND_pk_ix on CONDITIONS(IBIT) ),
 TEXT varchar2(64),
 DIMSERVICE varchar2(130)
);


CREATE or replace FUNCTION SET_SEPARATOR RETURN VARCHAR2 AS
BEGIN
 RETURN '_$';
END;
/

CREATE or replace FUNCTION SUBTITSTRING(subtit VARCHAR2) RETURN VARCHAR2 AS
BEGIN
   IF subtit is null then
   RETURN '';
   ELSE RETURN SET_SEPARATOR()||subtit;
   END IF;
END;
/

CREATE or replace FUNCTION TIMEST2UXT(ts IN TIMESTAMP) return int as
 mydate date := ts;
BEGIN
 RETURN (mydate-to_date('1970-01-01','YYYY-MM-DD'))*(86400);
END;
/


CREATE or replace FUNCTION parlength(pars vparameters) return int as
begin
if (pars is null) then
 return 0;
else
 return pars.COUNT;
end if;
end;
/

CREATE or replace FUNCTION parcomponent(pars vparameters, i int) return VARCHAR2 as
out VARCHAR2(15) := NULL;
begin
if (pars is not null) then
 if ( i <= pars.COUNT and i>0) then
  out := pars(i);
 end if;
end if;
return out;
end;
/

CREATE or replace FUNCTION vlabel(pars labels, i int) return VARCHAR2 as
out VARCHAR2(30) := NULL;
begin
if (pars is not null) then
 if ( i <= pars.COUNT and i>0) then
  out := pars(i);
 end if;
end if;
return out;
end;
/

create or replace view VIEWHISTOGRAM(NAME,HID,HSID,IHS,NHS,HSTYPE,SUBTITLE,TITLE,ALGO,TASK) as
  SELECT NAME,HID,HSID,IHS,NHS,HSTYPE,SUBTITLE,HSTITLE,HSALGO,HSTASK
  from HISTOGRAM H, HISTOGRAMSET HS where H.HSET = HS.HSID;

@subsystems;
