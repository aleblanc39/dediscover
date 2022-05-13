!
! Trust-region reflective 
!
![1] Coleman, T.F. and Y. Li, "An Interior, Trust Region Approach for Nonlinear Minimization Subject to Bounds," SIAM Journal on Optimization, Vol. 6, pp. 418–445, 1996.
![2] Coleman, T.F. and Y. Li, "On the Convergence of Reflective Newton Methods for Large-Scale Nonlinear Minimization Subject to Bounds," Mathematical Programming, Vol. 67, Number 2, pp. 189-224, 1994.
!
!Author : P.Linel
!



module sfmin
use iso_c_binding
use tr
implicit none
double precision :: inf



contains
subroutine c_nls_stir(x,lb,ub,c_fun,n,m,tolx,tolf,maxiter,obj) bind(c)
real(c_double),dimension(*),target,intent(in) :: x,lb,ub
integer(c_int),value :: n,m,maxiter
integer(c_long) :: obj
real(c_double),value :: tolx,tolf
double precision,dimension(:),pointer,contiguous :: xx,llb,uub
double precision,dimension(:),allocatable :: f
interface
        subroutine c_fun(x,f,obj) bind(c)
                use iso_c_binding
                real(c_double) :: x(*),f(*)
                integer(c_long) :: obj
        end subroutine
end interface
xx=>x(1:n)
llb=>lb(1:n)
uub=>ub(1:n)
!print *,'xx',xx
!print *,'llb',llb
!print *,'uub',uub

call nls_stir(xx,llb,uub,fun3,tolx,tolf,maxiter)
!print *,'xx result',x(1:n)

contains
subroutine fun3(x,f) 
          double precision,dimension(:),target,intent(in) :: x
          double precision,dimension(:),target,allocatable :: f
          real(c_double),pointer,dimension(:) :: c_x,c_f
          integer(c_int) :: i
          integer :: n
          
          n=size(x)
          !allocate(c_x(n))
          c_x=>x(1:n)
          if(.not.allocated(f)) allocate(f(m))
          c_f=>f!(1:n)
          call c_fun(c_x,c_f,obj)
end subroutine

end subroutine c_nls_stir

subroutine nls_stir(x,lb,ub,fun3,tol2,tol1,maxiter)

interface
subroutine fun3(x,f) 
          double precision,dimension(:),target,intent(in) :: x
          double precision,dimension(:),allocatable,target :: f
  end subroutine

end interface

double precision,dimension(:) :: x,lb,ub
double precision :: ifun,pcgtol,newval,val,degen
double precision,dimension(:,:),allocatable :: Z,A,newA
double precision,dimension(:),allocatable :: g,newgrad,newx,gopt,r,D,sx,snod
double precision,dimension(:),allocatable :: dv,v,dnewt,fvec,newfvec
integer :: n,m,iter,nfunevals,pcflags,maxiter,maxfunevals,kmax,posdef,npcg,oposdef
integer :: pcgit,ngradevals
logical :: nbnds,done,welldef
double precision :: qpval,diff,delta,tol1,tol2,ratio,nrmsx,oval,delbnd,gnrm,aug,theta
character(50) :: precontype
precontype='jacobprecon'
inf= 10**300 !1d0/0d0!ieee_value(1d0,ieee_positive_inf)


n = size(x) 
iter = 0 
nfunevals = 1  
ngradevals= 1
if (size(lb)==0) then
   lb = -inf
else
    
    where(lb <= -1e10) lb = -inf
end if
if (size(ub)==0) then
   ub = inf 
else
    
    where(ub >= 1e10) ub = inf
 end if

if (any(ub == lb)) then 
    print *,'fmin_stir:InvalidBounds'
    return
end if

pcflags = inf 
tol2 = 1d-6
tol1 = 1d-9
maxiter = 1000 
maxfunevals = 100*n 
pcgtol = 0.1d0 
kmax = 1
kmax = max(1,floor(dble(n/2)))


done = .false. 
posdef = 1
npcg = 0
pcgit = 0
delta = 10d0
nrmsx = 1d0
ratio = 0d0
oval = inf
pcflags=huge(1)
allocate(g(n),newgrad(n))
g=0d0
newgrad=0d0
nbnds=.false.
if (all( (lb == -inf) .and. (ub == inf) )) then
        degen = -1d0 
        nbnds=.true.
end if


call fun3(x,fvec)
m=size(fvec)
allocate(A(m,n))
call jac_approx(fun3,x,fvec,A)

delbnd = max(100d0*norm(x),1d0)


g = matmul(transpose(A),fvec)
val = dot_product(fvec,fvec)
print *,'val',val
allocate(v(n),dv(n),gopt(n),r(n),d(n),newx(n),newA(m,n),sx(n),snod(n))

do while (.not.done)
    
    call definevdv(g,x,lb,ub,v,dv)
    gopt = v*g
    gnrm = maxval(abs(gopt))
    r = abs(min(ub-x,x-lb))
    degen = minval(r + abs(g))
    if (nbnds) then
        degen = -1d0 
    end if
           
        diff = abs(oval-val) 
    oval = val 
    if ((gnrm < tol1) .and. (posdef == 1) ) then
        done = .true. 
        print *,'TolFun exit'
    elseif ((nrmsx < .9*delta) .and. (ratio > .25) .and. (diff < tol1*(1+abs(oval)))) then
        done = .true. 
        print *,'normal exit'
    elseif ((iter > 1) .and. (nrmsx < tol2)) then 
        done = .true. 
        print *,'TolX exit'
    elseif (nfunevals > maxfunevals) then
        done = .true. 
        print *,'MaxFunEval exit'
    elseif (iter > maxiter) then
        done = .true. 
        print *,'MaxIter exit'
    end if

    if (.not.done) then
        D = sqrt(abs(v))
        theta = max(.95d0,1d0-gnrm) 
        oposdef = posdef
        call tr_trial(x,g,A,D,delta,dv,pcflags,pcgtol,kmax,theta,lb,ub,&
                Z,dnewt,precontype,sx,snod,qpval,posdef,pcgit)
                nrmsx=norm(snod) 
        npcg=npcg + pcgit
        newx=x + sx
        call pertubtrr(newx,lb,ub)
        call fun3(newx,newfvec)
        nFunEvals = nFunEvals + 1
        newval = dot_product(newfvec,newfvec)
        
        if(.not.is_finite(newval)) then
                welldef=.false.
        else
                welldef=.true.
        end if

        if (.not.welldef) then 
            delta = min(nrmsx/20d0,delta/20d0)
        else       
            call jac_approx(fun3,newx,newfvec,newA)
            newgrad = matmul(transpose(newA),newfvec)
            aug = 0.5d0*dot_product(snod,dv*abs(newgrad)*snod)
            ratio = (newval + aug -val)/qpval
            
            if ((ratio >= .75d0) .and. (nrmsx >= 0.9d0*delta)) then
                delta = min(delbnd,2d0*delta)
            elseif (ratio <= .25d0 )  then
                delta = min(nrmsx/4d0,delta/4d0)
            !elseif (ratio <= 0) then !implementation papier
             !       delta=0.0625d0*delta
            end if
            if (newval < val) then
                    print *,'val',iter+1,newval
                    !print *,'       #$%#$%#$%#$% accept step #$%#$%#$%#$%',iter+1
                    x = newx 
                    val = newval 
                    g = newgrad
                    A = newA
                    deallocate(Z)

            end if
        end if
        iter = iter + 1
    end if
end do



!print *,'fval',val
!print *,'x',x
contains
        function is_finite(val) result(res)
                double precision,intent(in) :: val
                double precision :: v1
               logical :: res
               v1=val
               res=(val-v1) .eq. 0d0
       end function 
end subroutine
subroutine jac_approx(fun,x,f,jac)
double precision,dimension(:),intent(in) :: x,f
interface
        subroutine fun(x,g) 
                double precision,dimension(:),target,intent(in) :: x
                double precision,dimension(:),allocatable,target :: g
        end subroutine
end interface
double precision :: eps,fee
integer :: n,i,m
double precision,dimension(:),allocatable :: ei,fe
double precision,dimension(:,:),allocatable :: jac
eps=sqrt(epsilon(1d0))
n=size(x)
m=size(f)
allocate(ei(n),fe(m)) 
ei=0d0

do i=1,n
        ei(i)=1d0
        call fun(x+eps*(ei),fe)
        jac(:,i)=(fe-f)/(eps)
        ei(i)=0d0
end do
deallocate(ei,fe)
end subroutine


subroutine pertubtrr(x,l,u,idel,y,sigma,pert)
double precision,dimension(:),intent(in) :: l,u
double precision,optional :: idel
double precision,dimension(:) :: x
double precision,dimension(:),optional :: y,sigma
integer,optional :: pert
double precision :: del
if (.not.present(idel)) then
   del = 100d0*epsilon(1d0)
else
           del=idel
end if

if ((minval(abs(u-x)) < del) .or. (minval(abs(x-l)) < del)) then
   where( (u-x) < del) 
        x = x - del
   end where 
   where( (x-l) < del )
        x = x + del
   end where
   if (present(y) ) then
      where((u-x) < del)
        y = y - del*sigma
      end where
      where((x-l) < del)
        y = y + del*sigma
      end where
   end if
   if(present(pert)) pert = 1
else
   if(present(pert)) pert = 0
end if

end subroutine pertubtrr

subroutine definevdv(g,x,l,u,v,dv)
double precision,dimension(:),intent(in) :: g,x,l,u
double precision,dimension(:),allocatable :: v,dv
integer :: n
n = size(x); 
v = 0d0 
dv=0d0
where ((g < 0)  .and. (u <  inf )) 
        v  = (x - u) 
        dv = 1d0
end where 
where( (g >= 0) .and. (l > -inf))
        v  = (x - l) 
        dv = 1d0
end where
where((g < 0)  .and. (u == inf))
        v  = -1d0
        dv = 0d0
end where
where((g >= 0) .and. (l == -inf))
        v  = 1d0;
        dv = 0d0;
end where
end subroutine
end module sfmin
