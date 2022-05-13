!
! Author : P.Linel
!
!
module tr
implicit none
contains

subroutine  tr_trial(x,g,H,D,delta,dv,bandw,tol,kmax,theta,lb,ub,Z,dnewt,preconflag,&
                s,snod,qpval,posdef,pcgit)



double precision,dimension(:),intent(in) :: x,g,D,dv,lb,ub
double precision,dimension(:,:),intent(in) :: H
double precision,intent(in) :: delta,theta
integer,intent(in) :: kmax,bandw 
double precision,dimension(:),allocatable :: grad,v1,v2,dnewt,rhs,dis,nx,ngrad,sg,snod,ssg
double precision,dimension(:),allocatable :: ZZ,s,st,ss,ns,rr,nss,ssave,sssave,stsave,ssssave
double precision,dimension(:,:),allocatable :: R,W,Z,DM,DG,WW,MM
integer,dimension(:),allocatable :: permR
integer :: n,pcgit,posdef,i,po,fcnt,ipt(1),stat
double precision :: tol,inf,tol2,qpval1,qpval2,qpval3,nrmv2,qpval0
double precision :: qpval,alpha,mmdis,mdis,lambda,nst,nrhs,dtmp
character(50) :: preconflag
n = size(g)  
pcgit = 0 
allocate(grad(1:size(g)),source=g)
grad = D*g
allocate(DM (n,n))
allocate(DG(n,n),ZZ(n))
DG=0d0
DM=0d0
do i=1,n
DG(i,i) = abs(g(i))*dv(i)
DM(i,i)=D(i)
end do
posdef = 1 
tol2 = sqrt(epsilon(1d0))
if(allocated(dnewt)) allocate(v1(1:size(dnewt)),source = dnewt)  
inf = 10**300 !ieee_value(1d0,ieee_positive_inf)

qpval1 = inf 
qpval2 = inf 
qpval3 = inf

if (.not.allocated(Z)) then
   if (.not.allocated(v1)) then
      if (trim(preconflag)=='jacobprecon') then
         call aprecon(H,bandw,DM,DG,R,permR) 
      else
         print *,'tr_trial:invalidpreconflag'
      end if
      if (tol <= 0) tol = 0.1d0 
      
      call pcgr(D,diag(DG),grad,kmax,tol,H,R,permR,v1,posdef,pcgit,preconflag)
   end if
   if (norm(v1) > 0d0) then
      v1 = v1/norm(v1)
   end if
   allocate(Z(size(v1),2))
   allocate(v2(size(v1)))
   v2=0d0
   Z(:,1) = v1
   if (n > 1) then
      if (posdef < 1) then
         v2 = D*sign(1d0,grad) 
         if (norm(v2) > 0d0) then
            v2 = v2/norm(v2)
         end if
         v2 = v2 - v1*dot_product(v1,v2) 
         nrmv2 = norm(v2)
         if (nrmv2 > tol2) then
            v2 = v2/nrmv2 
            Z(:,2) = v2
         end if
      else
         if (norm(grad) > 0d0) then
            v2 = grad/norm(grad)
         else
            v2 = grad
         end if
         v2 = v2 - v1*dot_product(v1,v2) 
         nrmv2 = norm(v2);
         if (nrmv2 > tol2) then
            v2 = v2/nrmv2 
            Z(:,2) = v2
         end if
      end if
   end if
end if
allocate(W(n,2),WW(n,2))
allocate(rhs(2))
allocate(MM(2,2))
W = matmul(DM,Z)   
if(preconflag=='jacobprecon') then
   WW = matmul(transpose(H),matmul(H,W)) 
else
   print *,'tr_trial:invalidpreconflag'
end if

W=matmul(DM,WW) 
MM = matmul(transpose(Z),W) + matmul(transpose(Z),matmul(DG,Z)) 
rhs = matmul(transpose(Z),grad)
allocate(st(2),ss(n),ssave(n),sssave(n),stsave(2),ssssave(n))
call trustregion_solve(rhs,MM,delta,st,qpval,po,fcnt,lambda)
ss = matmul(Z,st)  
s = abs(D)*ss 
ssave = s
sssave = ss
stsave = st

if (all(abs(s)==0d0)) then   
   alpha = 1d0
   mmdis = 1d0
   ipt = -1  
else
        allocate(dis(n))
   where(abs(s)>0)
        dis = max((ub-x)/s, (lb-x)/s)
   end where
        mmdis = minval(dis)
        ipt=minloc(dis)
        mdis = theta*mmdis
        alpha = min(1d0,mdis)
end if
s = alpha*s 
st = alpha*st 
ss = alpha*ss
qpval1 = dot_product(rhs,st) + dot_product(0.5d0*st,matmul(MM,st))

if (n > 1) then 
   qpval3 = inf 
   ssssave = mmdis*sssave
   if (norm(ssssave) < 0.9d0*delta) then
      deallocate(v2,v1,stat=stat)
      allocate(v1(n),v2(n))
      allocate(ns(n),nx(n),ngrad(n),nss(n),rr(n))
      rr = mmdis*ssave 
      nx = x+rr
      stsave = mmdis*stsave
      qpval0 = dot_product(rhs,stsave)+dot_product(0.5d0*stsave,matmul(MM,stsave))
      if(trim(preconflag)== 'jacobprecon') then
         v2 = matmul(transpose(H),matmul(H,rr)) 
      else
        print *,'tr_trial:invalidpreconflag'
      end if
      
      v2 = v2 + g 
      ngrad = D*v2
      ngrad = ngrad + matmul(DG,ssssave)
      nss = sssave 
      if(ipt(1).ne.-1) nss(ipt) = -nss(ipt) 
      ZZ = nss/norm(nss)
      v2 = D*ZZ 
      
      if(trim(preconflag)== 'jacobprecon') then
         v1= matmul(transpose(H),matmul(H,v2)) 
      else
        print *,'tr_trial:invalidpreconflag'
      end if
      
      
      v2 = D*v1
      dtmp = dot_product(ZZ,v2) + dot_product(ZZ,matmul(DG,ZZ))
      nrhs=dot_product(ZZ,ngrad)
      call quad1d(nss,ssssave,delta,nss,nst)
      nst = nst/norm(nss)
      ns = abs(D)*nss 

      if (all(abs(ns)==0d0)) then 
         alpha = 1d0
      else
        where(abs(ns)>0)
         dis = max((ub-nx)/ns, (lb-nx)/ns)
        end where
         mdis = minval(dis)  
         mdis = theta*mdis
         alpha = min(1d0,mdis) 
      end if
      ns = alpha*ns 
      nst = alpha*nst 
      nss = alpha*nss
      qpval3 = qpval0 +  nrhs*nst + 0.5d0*nst*dtmp*nst
   end if
   
   deallocate(v2,v1,stat=stat)
   allocate(v2(n),v1(n))
   dtmp = norm(grad)
   if(dtmp==0d0) dtmp=1d0
   ZZ = grad/dtmp 
   v2 = D*ZZ 
   
   if(trim(preconflag)=='jacobprecon') then
      v1 = matmul(transpose(H),matmul(H,v2)) 
   else
      print *,'tr_trial:invalidpreconflag'
   end if
   
   
   v2 = D*v1
   deallocate(MM)
   dtmp = dot_product(ZZ,v2) + dot_product(ZZ,matmul(DG,ZZ)) 
   allocate(MM(1,1))
   allocate(ssg(n),sg(n))
   MM=dtmp
   rhs = dot_product(ZZ,grad)
   call trustregion_solve(rhs(1:1),MM,delta,st,qpval,po,fcnt,lambda)
   ssg = ZZ*st(1) 
   sg = abs(D)*ssg
   
   
   if (all(abs(sg)==0d0)) then
      alpha = 1d0
   else
      where(sg>0)
      dis = max((ub-x)/sg, (lb-x)/sg)
      end where
      mdis = minval(dis) 
      mdis = theta*mdis
      alpha = min(1d0,mdis) 
   end if
   sg = alpha*sg 
   st = alpha*st 
   ssg = alpha*ssg
   qpval2 = rhs(1)*st(1) + (0.5d0*st(1))*MM(1,1)*st(1)
end if


if (qpval2 <= min(qpval1,qpval3)) then
   qpval = qpval2 
   s = sg 
   snod = ssg
elseif (qpval1 <= min(qpval2,qpval3)) then
   qpval = qpval1 
   snod = ss
else
   qpval = qpval3 
   s = ns + rr 
   snod = nss + ssssave
end if
end subroutine tr_trial
subroutine aprecon(A,upperbandw,DM,DG,R,pvec)
double precision,dimension(:,:),intent(in) :: A,DM,DG 
double precision,dimension(:,:),allocatable :: R,DDG,TM
double precision,dimension(:),allocatable :: epsi,ddiag,dnrms
integer,dimension(:),allocatable :: pvec
integer :: info,i,n,rows,cols,upperbandw,m
double precision :: mind,lambda,mdiag
integer :: lwork
double precision,dimension(:),allocatable :: tau,work

rows=size(A,1)
m=rows
cols=size(A,2)
n = size(DM,1)
allocate(R(n,n),epsi(n))
do i=1,n
R(i,i) =1d0 
end do
allocate(pvec(n))


epsi = sqrt(epsilon(1d0))

allocate(TM(1:size(A,1),1:size(A,2)),source=A)
TM = matmul(A,DM)

if (upperbandw == 0) then 
   allocate(DDG(1:size(DG,1),1:size(DG,2)),source=DG)
   DDG = matmul(transpose(TM),TM) + DG
   allocate(dnrms(n))
   dnrms = sqrt(sum(DDG**2,dim=1))
   dnrms = max(sqrt(dnrms),epsi)
   do i=1,n
        R(i,i) = dnrms(i)
   end do
   pvec = [(i,i=1,n)]
elseif (upperbandw >= n-1) then 
   allocate(DDG(m+n,n))
   DDG(m+1:m+n,1:n) = sqrt(DG)
   DDG(1:m,1:n)=TM
   lwork=n+1
   allocate(tau(n),work(lwork))
   call dgeqrf(m+n, n, DDG, m+n, tau, work, lwork, info)
   R(1:n,1:n)=DDG(1:n,1:n)
   do i=1,n
   R(i+1:n,i)=0d0
   end do
   pvec=[(i,i=1,n)]
   
   mdiag = minval(abs(diag(R)))
   lambda = 1d0
   do while (mdiag < sqrt(epsi(1)))
      DDG=0d0
      R=0d0
      DDG(1:m,1:n) = TM
      do i=1,n
      R(i,i)=1d0
      end do
      DDG(m+1:m+n,1:n)=sqrt(DG) + lambda*R
      lambda = 4d0*lambda
      call dgeqrf(m+n, n, DDG, m+n, tau, work, lwork, info)
      R(1:n,1:n)=DDG(1:n,1:n)
      do i=1,n
      R(i+1:n,i)=0d0
      end do
      mdiag = minval(abs(diag(R)));
   end do
   
elseif ((upperbandw > 0) .and. (upperbandw < n-1)) then 

print *,'not implemented'
stop
else
   print *,'aprecon:invalidupperbandw'
end if
end subroutine aprecon
               

subroutine pcgr(DM,DG,g,kmax,tol,H,R,pR,p,posdef,k,flag)
double precision,dimension(:,:),intent(in) :: H,R
double precision,dimension(:),intent(in) :: g,DM,DG
double precision,dimension(:),allocatable :: rr,p,d,ww,z,w
double precision :: alpha,beta,denom,tol,stoptol,inner2,inner1,znrm
integer,dimension(:) ::  pR
integer :: posdef,kmax,k,n
character(50) :: flag
n = size(DG,1)
allocate(rr(1:size(g)) ,source =-g)
allocate(p(n),ww(n),w(n))
allocate(z(n),d(n))
p = 0d0 
z = preproj(rr,R,pR)
znrm = sqrt(dot_product(z,z)) 
stoptol = tol*znrm
inner2 = 0d0 
inner1 = dot_product(rr,z)
posdef = 1
kmax = max(kmax,1)  
k=1
do k =1,kmax
   if (k==1) then
      d = z
   else
      beta = inner1/inner2
      d = z + beta*d
   end if
   ww = DM*d
   if(trim(flag)=='jacobprecon') then
      w = matmul(transpose(H),matmul(H,ww)) 
   else
      print *,'pcgr:wrongcallingF' 
   end if
   ww = DM*w +DG*d
   denom = dot_product(d,ww)
   if (denom <= 0) then
      if (sqrt(dot_product(d,d)) == 0d0) then
        p = d
      else      
              p = d/sqrt(dot_product(d,d))
      end if
      posdef = 0
      exit
   else
      alpha = inner1/denom
      p = p + alpha*d
      rr = rr - alpha*ww
   end if
   z = preproj(rr,R,pR)
   
   if (sqrt(dot_product(z,z)) <= stoptol) exit 
   inner2 = inner1
   inner1 = dot_product(rr,z)
end do
if(k==kmax+1) k=k-1

deallocate(rr,ww,w,d,z)
contains

function preproj(r,RPJ,ppvec) result(w)
double precision,dimension(:) :: r
double precision,dimension(:),allocatable :: wbar,w
double precision,dimension(:,:) :: RPJ
integer,dimension(:) :: ppvec
integer :: n
n = size(r)
allocate(wbar(n),w(n))
call directlu(wbar, RPJ,r(ppvec),.false.,'T')
call directlu(w , RPJ,wbar,.true.,'N')
w=w(ppvec)
end function 
end subroutine

subroutine directlu(x,mat,b,isfact,mode)
double precision,dimension(:),intent(out) :: x
double precision,dimension(:),intent(in) :: b
double precision,dimension(:,:),intent(inout) :: mat 
integer,dimension(:),allocatable,save :: pivot
double precision,dimension(:),allocatable :: tmpb
logical,intent(in) :: isfact
integer :: n,m,i,info
double precision :: anorm,rcond
character(1) :: mode

n=size(mat,1)
m=size(mat,2)
allocate(tmpb(size(b,1)))
tmpb=b
if (.not. isfact ) then
        if (allocated(pivot)) deallocate(pivot)
        allocate(pivot(min(m, n)))
        pivot=0
        !mat2=mat
        !print *,size(mat),size(mat2)
        !anorm=0d0
        !do i=1,n
        !anorm=max(anorm,sum(abs(mat(i,:))))
        !end do
        !print *,'anorm',anorm
        !call getrf(mat,pivot,info)
        call dgetrf(n,m,mat,n,pivot,info)
        !print *,'factorized',info
        if (info>0) then 
                print *,'matrix singular',info
                stop
        end if
        !call gecon( mat2, anorm, rcond)
        !print *,'rcond',rcond
        
        !print *,'pivot',pivot
        !pause
        !call getrs(mat,pivot,tmpb)
        call dgetrs(mode,m,1,mat,n,pivot,tmpb,n,info)
        !print *,'solved',info
        !print *,"residu LU ",norm(b-matmul(mat2,x))
        !pause
        !deallocate(pivot)
else
        call dgetrs(mode,m,1,mat,n,pivot,tmpb,n,info)
        !print *,'solved',info
end if
x=tmpb(1:m)
deallocate(tmpb)
end subroutine directlu
  
subroutine quad1d(x,ss,delta,nx,tau)
double precision,dimension(:),intent(in) :: x,ss
double precision,intent(in) ::  delta 
double precision :: a,b,c,numer,r1,r2,tau
double precision,dimension(:),allocatable :: nx

a = dot_product(x,x)
b = 2d0*dot_product(ss,x) 
c = dot_product(ss,ss)-delta**2

numer = -(b + sign(1d0,b)*sqrt(b**2-4d0*a*c))
r1 = numer/(2d0*a)
r2 = c/(a*r1)

tau = max(r1,r2) 
tau = min(1d0,tau)
if (tau <= 0) print *,'tr_trial:sqrrt negative root' 
if(.not.allocated(nx)) allocate(nx(size(x,1)))
nx = tau*x
end subroutine quad1d

subroutine trustregion_solve(g,H,delta,s,val,posdef,cnt,lambda)
double precision,dimension(:),intent(in) :: g
double precision,dimension(:,:),intent(in) :: H
double precision,intent(in) :: delta
double precision :: tol,tol2,mineig,nrms,laminit,val,lambda,vval,b,beta
integer :: key,itbnd,n,cnt,jmin,sig,posdef,liwork,lwork
double precision,dimension(:),allocatable :: coeff,s,work,eigval,alpha,w
double precision,dimension(:,:),allocatable :: eigV
integer,dimension(:),allocatable :: iwork
integer :: minj(1),info,stat

tol = 1d-12 
tol2 = 1d-8 
key = 0
itbnd = 50
lambda = 0
n = size(g) 
allocate(coeff(n))
deallocate(s,stat=stat)
allocate(s(n))
coeff=0d0 
lwork=2*n**2+ 6*n + 1
liwork=5*n+3
allocate(work(lwork),iwork(liwork),eigval(n))
allocate(eigV(1:size(H,1),1:size(H,2)),source=H)
call dsyevd('V', 'L', n, eigV, size(eigV,1), eigval, work, lwork, iwork, liwork, info)
if (info.ne.0) print *,'info eigVD',info
deallocate(work,iwork)
cnt = 0 
mineig = minval(eigval)
minj=minloc(eigval)
jmin=minj(1)
allocate(alpha(n))
alpha = -matmul(transpose(eigV),g)
sig=0
if (alpha(jmin)==0d0) sig=1
sig = sign(1d0,alpha(jmin)) + sig


if (mineig > 0) then
   coeff = alpha / eigval
   lambda = 0
   s = matmul(eigV,coeff) 
   posdef = 1
   nrms = sqrt(dot_product(s,s))
   if (nrms <= 1.2d0*delta) then
      key = 1
   else 
      laminit = 0d0
   end if
else
   laminit = -mineig 
   posdef = 0
end if

allocate(w(n))
if (key == 0) then
   if (seceqn(laminit,eigval,alpha,delta) > 0d0) then
      call rfzero(seceqn,laminit,itbnd,eigval,alpha,delta,tol,b,vval,cnt)    
      vval = abs(vval) ! A MONITORER IMPROVMNT
      if (vval <= tol2) then
         lambda = b 
         key = 2
         w = eigval + lambda
         where (w.ne.0d0) 
                coeff = alpha/w
         end where
         where (w==0d0)
                 where( alpha==0d0) 
                         coeff = 0d0
                end where
                where (alpha.ne.0d0) 
                        coeff = 10**300 !1d0/0d0!ieee_value(1d0,ieee_positive_inf)
                end where
         end where
         where (isnan(coeff))
                coeff=0
         end where
         s = matmul(eigV,coeff) 
         nrms = sqrt(dot_product(s,s))
         if ((nrms > 1.2d0*delta) .or. (nrms < 0.8d0*delta)) then
            key = 5 
            lambda = -mineig
        end if
      else
         lambda = -mineig 
         key = 3
      end if
   else
      lambda = -mineig
      key = 4
   end if

   if (key > 2) then 
           where (abs(eigval + lambda) < 10d0 * epsilon(1d0) * max(abs(eigval),1d0))
                alpha = 0d0
           end where
           w = eigval + lambda;
           where (w.ne.0d0) 
                   coeff = alpha/ w
           end where
           where (w==0d0)
                   where((alpha == 0d0))
                        coeff = 0d0
                   end where
                   where (alpha.ne.0d0) 
                           coeff = 10**300 !1d0/0d0!ieee_value(1d0,ieee_positive_inf)
                   end where
           end where
           where (isnan(coeff))
                   coeff=0
           end where
           s = matmul(eigV,coeff) 
           nrms = sqrt(dot_product(s,s))
           if ((key > 2) .and. (nrms < 0.8d0*delta)) then
                   beta = sqrt(delta**2 - nrms**2)
                   s = s + beta*sig*eigV(:,jmin)
           end if
           if ((key > 2) .and. (nrms > 1.2*delta)) then
                   call rfzero(seceqn,laminit,itbnd,eigval,alpha,delta,tol,b,vval,cnt)
                   lambda = b 
                   w = eigval + lambda
                   where (w.ne.0d0) 
                           coeff = alpha/ w
                   end where
                   where (w==0d0) 
                           where((alpha == 0d0))
                                   coeff = 0d0
                           end where
                           where (alpha.ne.0d0) 
                                   coeff = 10**300!1d0/0d0!ieee_value(1d0,ieee_positive_inf)
                           end where
                   end where
                   where (isnan(coeff))
                           coeff=0
                   end where
                   s = matmul(eigV,coeff) 
                   nrms = sqrt(dot_product(s,s))
           end if
   end if
end if

val=dot_product(g,s)+0.5d0*dot_product(s,matmul(H,s))
deallocate(alpha,coeff,eigV,eigval,w)
contains

function  seceqn(lambda,eigval,alpha,delta) result(value)

double precision,intent(in) :: lambda,delta
double precision,dimension(:),intent(in) :: eigval,alpha
double precision :: value,unm
double precision,dimension(:),allocatable :: unn
double precision,dimension(:),allocatable :: matM,MC,MM
integer :: m,n
m = 1
n = size(eigval)
allocate(unn(n),matM(n),MM(n),MC(n))
unn = 1d0
unm = 1d0
matM = eigval*unm + unn*lambda
MC = matM
MM = alpha*unm
where(matM.ne.0d0)
       matM= MM / matM
end where
!where (MC==0d0)
!       matM =1d0/0d0 ! ieee_value (1d0, ieee_positive_inf)
!end where 
matM = matM**2
value = sqrt(unm / dot_product(matM,unn))
if(isnan(value)) value = 0d0
value = (1d0/delta)*unm - value
deallocate(unn,matM,MM,MC)
end function seceqn

subroutine rfzero(fun,x,itbnd,eigval,alpha,delta,tol,b,fb,itfun)
implicit none
interface
        function  fun(lambda,eigval,alpha,delta) result(value)
                double precision,intent(in) :: lambda,delta
                double precision,dimension(:),intent(in) :: eigval,alpha
                double precision :: value
        end function 
end interface

double precision,intent(in) :: x,delta,tol
integer,intent(in) :: itbnd
double precision,dimension(:),intent(in) :: eigval,alpha
integer :: itfun
double precision :: dx,a,fa,b,fb,c,fc,d,e
double precision :: m,toler,r,s,p,q
itfun = 0
if (x.ne. 0d0) then 
    dx = 0.5d0*abs(x)
else
    dx = 0.5d0
end if
a = x
c = a
fa = fun(a,eigval,alpha,delta)
itfun = itfun+1
b = x + 1d0
fb = fun(b,eigval,alpha,delta)
itfun = itfun+1


do while ((fa > 0d0) .eqv. (fb > 0d0))
   dx = 2d0*dx
   if ((fa > 0d0) .neqv. (fb > 0d0))  exit
   b = x + dx  
   fb = fun(b,eigval,alpha,delta)
   itfun = itfun+1
   if (itfun > itbnd) exit
end do

fc = fb
do while (fb .ne. 0)
   if ((fb > 0) .eqv. (fc > 0)) then
      c = a  
      fc = fa
      d = b - a
      e = d
   end if
   if (abs(fc) < abs(fb)) then
      a = b 
      b = c 
      c = a
      fa = fb 
      fb = fc 
      fc = fa
   end if
   
   if (itfun > itbnd) exit
   m = 0.5d0*(c - b)
   toler = 2d0*tol*max(abs(b),1d0)
   if ((abs(m) <= toler) .or. (fb == 0d0)) exit
   
   if ((abs(e) < toler) .or. (abs(fa) <= abs(fb))) then
      d = m  
      e = m
   else
      s = fb/fa
      if (a == c) then
         p = 2d0*m*s
         q = 1d0 - s
      else
         q = fa/fc
         r = fb/fc
         p = s*(2d0*m*q*(q-r)-(b-a)*(r-1d0))
         q = (q-1d0)*(r-1d0)*(s-1d0)
      end if
      if (p > 0d0) then
              q = -q
      else 
              p = -p 
      end if
      if ((2d0*p < 3d0*m*q - abs(toler*q)) .and. (p < abs(0.5d0*e*q))) then
         e = d 
         d = p/q
      else
         d = m
         e = m
      end if
   end if 
   
   a = b
   fa = fb
   if (abs(d) > toler) then 
           b = b + d
   else
           if (b > c) then 
            b = b - toler
           else
            b = b + toler
           end if
   end if
   fb = fun(b,eigval,alpha,delta)
   itfun = itfun + 1
end do 
end subroutine rfzero

end subroutine trustregion_solve
function diag(A)
        double precision,dimension(:,:) :: A
        double precision,dimension(:),allocatable :: diag
        integer :: i
        allocate(diag(size(A,1)))
        do i=1,size(A,1)
        diag(i)=A(i,i)
        end do
end function diag
function norm(v) result(nm)
        double precision,dimension(:),intent(in) :: v
        double precision :: nm
        nm=sqrt(dot_product(v,v))
end function norm
end module tr
