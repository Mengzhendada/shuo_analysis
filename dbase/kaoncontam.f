      program kaoncontam
c calculate fraction of kaons that decay to pi or mu before aerogel
c and would pass the pion RF time cut as a function of P
      implicit none
      integer i,j,k,jj,kk
      real*8 a,b,prob(22),mk/0.49/,beta,dt0,dt,sigma,prb
      real*8 sum1,sum2,sumall,dist,p,sumallwid
    
c width of RF peak in nsec (0.2 from Shuo, sqrt of 2 because not in formula
      sigma = 0.20 * 1.414

c read in decays versus shms distance using high statistics copy
c of cntdecay from runs 3470-72
      open(unit=5,file='kaoncontam.inp')
      prob(22) = 1.0
      do i=1,21
       read(5,'(2i3,2f7.3)') j,k,a,b
       prob(i) = (a + b)/2.
       prob(22 ) = prob(22) - prob(i)
      enddo

      do i=1,10
       p = 1. + 0.5*i
       beta = p / sqrt(p**2 + mk**2)
       sumall = 0.
       sumallwid = 0. ! with pid like aerogeol and Heavy gas
       do j=1,22
        dist = float(j)-0.5 ! distance as a kaon
        dt0 = dist  * 3. * (1/beta - 1)
        sum1 = 0.
        sum2 = 0.
        do k=1,100
         a = (-4. + 8.*(k-0.5)/100.) 
         prb = exp(-a**2 / sigma**2)
         dt = dt0 + a * sigma
         sum1 = sum1 + prb
c assumes cut on pions is 0.5 nsec
         if(dt.lt.0.5) sum2 = sum2 + prb
        enddo
c        write(6,'(8f7.3)') dist,dt0,prob(j),sum2/sum1
        sumall = sumall + prob(j) * sum2/sum1 
        if(j.le.21) sumallwid = sumallwid + prob(j) * sum2/sum1 
       enddo
       write(6,'(''total'',3f7.3)') p,sumall,sumallwid
      enddo
      stop
      end

