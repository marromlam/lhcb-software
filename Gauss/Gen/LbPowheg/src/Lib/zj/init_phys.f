      subroutine zj_init_phys
      implicit none
      include 'src/Lib/zj/nlegborn.h'
      include 'src/Lib/zj/pwhg_flst.h'
      include 'src/Lib/zj/pwhg_kn.h'
      include 'src/Lib/zj/pwhg_pdf.h'
      include 'src/Lib/zj/pwhg_st.h'
      include 'src/Lib/zj/pwhg_rad.h'
      include 'src/Lib/zj/pwhg_dbg.h'
      include 'src/Lib/zj/pwhg_flg.h'
      include 'src/Lib/zj/pwhg_par.h'
      include 'src/Lib/zj/pwhg_physpar.h'
      character * 5 zj_scheme
      character * 3 zj_whichpdfpk
      real * 8 zj_powheginput
      integer iorder,iret,iun,j
      external zj_whichpdfpk,zj_powheginput
c Initialization of default values for common block
c variables. These may be overridden by the user program
c init_processes.
      par_diexp=1
      par_dijexp=1
      par_2gsupp=1
c
      par_isrtinycsi = 1d-6
      par_isrtinyy = 1d-6
      par_fsrtinycsi = 1d-5
      par_fsrtinyy = 1d-6
c
      rad_branching=1
c this is set to true in processes where the FSR jacobian
c can become singular (massless recoil particle)
      flg_jacsing=.false.
c flag to use importance sampling in the x variable in
c collinear remnant generation. Needed for charm at LHC
      flg_collremnsamp=.false.
c End initialization of common block defaults.
      pdf_ih1=zj_powheginput('ih1')
      pdf_ih2=zj_powheginput('ih2')
      if(zj_whichpdfpk().eq.'lha') then
         pdf_ndns1=zj_powheginput('lhans1')
         pdf_ndns2=zj_powheginput('lhans2')
      elseif(zj_whichpdfpk().eq.'mlm') then
         pdf_ndns1=zj_powheginput('ndns1')
         pdf_ndns2=zj_powheginput('ndns2')
      else
         write(*,*) ' unimplemented pdf package',zj_whichpdfpk()
         stop
      endif
      if(pdf_ndns1.ne.pdf_ndns2) then
         st_lambda5MSB=zj_powheginput('QCDLambda5')
      else
         call zj_genericpdfpar(pdf_ndns1,pdf_ih1,st_lambda5MSB,
     1                      zj_scheme,iorder,iret)
         if(iret.ne.0) then
            write(*,*) ' faulty pdf number ',pdf_ndns1
            stop
         endif
      endif
      kn_beams(0,1)=zj_powheginput('ebeam1')
      kn_beams(0,2)=zj_powheginput('ebeam2')
      kn_beams(1,1)=0
      kn_beams(1,2)=0
      kn_beams(2,1)=0
      kn_beams(2,2)=0
      kn_beams(3,1)=kn_beams(0,1)
      kn_beams(3,2)=-kn_beams(0,2)
      kn_sbeams=4*kn_beams(0,1)*kn_beams(0,2)

c generation cut: see zj_gen_born_phsp.f
      kn_ktmin=zj_powheginput("#bornktmin")
      if(kn_ktmin.lt.0) kn_ktmin=0

c zj_masses for light fermions, used in momentum reshuffling
      do j=1,6
         physpar_mq(j)=0
      enddo
      do j=1,3
         physpar_ml(j)=0
      enddo

c thresholds 
      rad_ptsqmin=zj_powheginput('#ptsqmin')
      if(rad_ptsqmin.lt.0) rad_ptsqmin=0.8d0
      rad_charmthr2=zj_powheginput('#charmthr')
      if(rad_charmthr2.lt.0) rad_charmthr2=1.5d0
      rad_charmthr2=rad_charmthr2**2
      rad_bottomthr2=zj_powheginput('#bottomthr')
      if(rad_bottomthr2.lt.0) rad_bottomthr2=5d0
      rad_bottomthr2=rad_bottomthr2**2
c zj_scale factors
      st_renfact=zj_powheginput('#renscfact')
      st_facfact=zj_powheginput('#facscfact')
      if(st_facfact.lt.0) st_facfact=1
      if(st_renfact.lt.0) st_renfact=1

c     if true, perform the check that there are no coloured light
c     partons before flst_lightpart
      flg_lightpart_check=.true.
c
c initialize Lambda values for radiation
      call zj_init_rad_lambda
c
      call zj_init_processes

      call zj_init_couplings

c initialize number of singular regions
      rad_nkinreg=1+(nlegborn-flst_lightpart+1)
      call zj_genflavreglist


      dbg_softtest=.true.
      dbg_colltest=.true.
      if(flg_withdamp) then
         write(*,*) ' no zj_soft tests if withdamp is set'
         dbg_softtest=.false.
      endif
      if(flg_bornonly) then
         write(*,*) ' no zj_soft and coll. tests if bornonly is set'
         dbg_softtest=.false.
         dbg_colltest=.false.
      endif
      if (dbg_softtest.or.dbg_colltest) then         
         call zj_newunit(iun)
         open(unit=iun,file='pwhg_checklimits')
         call zj_checklims(iun)
         call flush(iun)
      endif
      end

