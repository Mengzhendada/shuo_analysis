      program kaoncontam
c calculate fraction of kaons that decay to pi or mu before aerogel
c and would pass the pion RF time cut as a function of P
      implicit none
      integer i,j,k,jj,kk,dk,icut,jp
      real*8 a,b,prob(22,10),mk/0.49/,beta,dt0,dt,sigma,prb
      real*8 sumall,dist,p,sumallwid,ctrf(40,3)
      character*80 string

c read in decays versus shms distance using high statistics copy
c of cntdecay from all runs summed together, in 10 momentum bins
c 1.75, 2.25, ...
      open(unit=5,file='kaoncontam.inp')
      read(5,'(a)') string
      read(5,'(a)') string
      do i=1,22
       read(5,'(3x,10f8.4)') (prob(i,j),j=1,10)
      enddo
c "pure" distributions for electrons
      do i=1,40
       read(5,'(3x,10f10.4)') (ctrf(i,j),j=1,3)
      enddo

c caculate kaons in pion ditrictuion for the 3 run periods
c with and without bin 22 (pure kaons). BIns 1-21 are all muons or pions
c from decays. 
      do jp=1,3
       if(jp.eq.1) icut=30
       if(jp.eq.2) icut=40
       if(jp.eq.3) icut=15

       do i=1,10
        p = 1.5 + 0.5*(i-0.5)
        beta = p / sqrt(p**2 + mk**2)
        sumall = 0.
        sumallwid = 0. ! with pid like aerogeol and Heavy gas
        do j=1,22
         dist = float(j)-0.5 ! distance as a kaon
         dt0 = dist  * 3. * (1/beta - 1)
         dk = int(dt0*10. + 0.5)
         do k=1,40
          if(k + dk .le.icut) then
           if(j.lt.22) sumallwid = sumallwid + ctrf(k,jp)*prob(j,i)
           sumall = sumall + ctrf(k,jp)*prob(j,i)
          endif
         enddo
        enddo
        write(6,'(i3,3f7.3)') jp, p,sumall,sumallwid
       enddo
      enddo
      stop
      end

