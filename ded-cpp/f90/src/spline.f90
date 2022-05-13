module spline
use iso_c_binding
!use ieee_arithmetic
implicit none

double precision,private,parameter :: PI = 3.141592653589793238462643d0
logical :: autoweight=.true.
type bspline
        double precision,dimension(:),allocatable :: knots,coefs
        integer :: order
end type bspline

type, bind(c) :: c_bspline
        type(c_ptr) :: knots,coefs
        integer(c_int) :: order,sizeknots,sizecoefs
end type c_bspline

type mat_aij
        integer,dimension(:),allocatable :: row,column
        double precision,dimension(:),allocatable :: values
        integer :: n,m,nnzeros
end type mat_aij

contains
function aijtofull(mat) result(full)
        type(mat_aij),intent(in) :: mat
        double precision,dimension(:,:),allocatable :: full
        integer :: i
        allocate(full(mat%n,mat%m))
        full=0d0
        if (mat%nnzeros==mat%n*mat%m) then

                do i=1,mat%n
                full(i,:)=mat%values((i-1)*mat%m+1:i*mat%m)
                end do
        else
                do i=1,mat%nnzeros
                full(mat%row(i),mat%column(i))=mat%values(i)
                end do
        end if

end function aijtofull

subroutine convert_full_to_aij(full,aij)
        type(mat_aij) ::aij
        double precision,dimension(:,:) :: full
        integer :: i,j,k

        aij%nnzeros=count(full.ne.0d0)
        aij%n=size(full,1)
        aij%m=size(full,2)
        if(.not.allocated(aij%values)) then
        allocate(aij%row(aij%nnzeros))
        allocate(aij%column(aij%nnzeros))
        allocate(aij%values(aij%nnzeros))
        end if
        k=0
        do j=1,aij%n
                do i=1,aij%m
                        if (full(j,i).ne.0d0) then
                                k=k+1
                                aij%row(k)=j
                                aij%column(k)=i
                                aij%values(k)=full(j,i)
                        end if
                end do
        end do    
end subroutine convert_full_to_aij

subroutine matmul_aijm(mat,x,res)
        type(mat_aij),intent(in) :: mat
        double precision,dimension(:,:),intent(in) ::x
        double precision, dimension(:,:),intent(out) :: res
        integer :: i,j
        res=0d0
        do j=1,size(x,2)
        do i=1,mat%nnzeros
                res(mat%row(i),j)=res(mat%row(i),j)+mat%values(i)*x(mat%column(i),j)
        end do
        end do
end subroutine matmul_aijm

subroutine matmul_aij(mat,x,res)
        type(mat_aij),intent(in) :: mat
        double precision, dimension(:),intent(in) ::x
        double precision, dimension(:),intent(out) :: res
        integer :: i
        res=0d0
        do i=1,mat%nnzeros
                res(mat%row(i))=res(mat%row(i))+mat%values(i)*x(mat%column(i))
        end do
end subroutine matmul_aij


subroutine c_spaps(t,y,tol,wi,order,f_sp,n,autweight, usegcv) bind(c)
implicit none

type(c_bspline) :: f_sp
real(c_double),dimension(*),target,intent(in) :: t,y,wi
real(c_double),value :: tol
integer(c_int),value :: n,order, usegcv
integer(c_int),value :: autweight
integer :: i
type(bspline) :: ff_sp

double precision :: gcv = 0
double precision,dimension(:),pointer :: tt,yy,wii 
real(c_double),dimension(:), pointer :: temp1,temp2

tt=>t(1:n)
yy=>y(1:n)
wii=>wi(1:n)
if(autweight .eq. 0) autoweight=.false.

!print *, tt, yy
print *, 'c_spaps: value of order: ', order 

if (usegcv .eq. 0) then
  ! print *, 'not using gcv'
   call autospaps(tt,yy,order,ff_sp)
else
 !  print *, 'using gcv'
    call spaps(tt,yy,tol,wii,order,ff_sp, gcv)

end if



!call autospaps(tt,yy,order,ff_sp)
!call spaps(tt,yy,tol,wii,order,ff_sp)
print *, 'allocating'
f_sp%order=ff_sp%order
f_sp%sizeknots=size(ff_sp%knots,1)
f_sp%sizecoefs=size(ff_sp%coefs,1)
allocate(temp1(f_sp%sizeknots))
allocate(temp2(f_sp%sizecoefs))
print *, 'allocated'

temp1=ff_sp%knots
temp2=ff_sp%coefs
f_sp%knots=c_loc(temp1(1)) 
f_sp%coefs=c_loc(temp2(1))
print *, 'deallocating'

deallocate(ff_sp%knots)
deallocate(ff_sp%coefs)
print *, 'deallocated'


end subroutine c_spaps

function c_bsp_eval(sp,x) bind(c) 
        type(c_bspline),intent(in) :: sp
        type(bspline) :: f_sp
        real(c_double),intent(in) :: x
        real(c_double) :: c_bsp_eval
        real(c_double),dimension(:),pointer :: knots,coefs

        f_sp%order=sp%order
        !print*,'order',f_sp%order
        call c_f_pointer(sp%knots,knots,(/sp%sizeknots/))
        call c_f_pointer(sp%coefs,coefs,(/sp%sizecoefs/))
        allocate(f_sp%knots(sp%sizeknots),source=knots)
        allocate(f_sp%coefs(sp%sizecoefs),source=coefs)
        !print*,'knots',f_sp%knots
        !print*,'coefs',f_sp%coefs
        c_bsp_eval=bsp_evalbv(f_sp,x)
end function c_bsp_eval

function c_bsp_evald(sp,x) bind(c) 
        type(c_bspline),intent(in) :: sp
        type(bspline) :: f_sp
        real(c_double),intent(in) :: x
        real(c_double) :: c_bsp_evald
        real(c_double),dimension(:),pointer :: knots,coefs

        f_sp%order=sp%order
        !print*,'order',f_sp%order
        call c_f_pointer(sp%knots,knots,(/sp%sizeknots/))
        call c_f_pointer(sp%coefs,coefs,(/sp%sizecoefs/))
        !print*,'knots',sp%sizeknots
        !print*,'coefs',sp%sizecoefs

        allocate(f_sp%knots(sp%sizeknots),source=knots)
        allocate(f_sp%coefs(sp%sizecoefs),source=coefs)
        c_bsp_evald=bvalue(f_sp%knots,f_sp%coefs,size(f_sp%coefs), f_sp%order, x, 1 )
end function c_bsp_evald

subroutine autospaps(ti,yi,order,f_sp)
use goldenpara
        double precision,dimension(:),intent(in) :: ti,yi
        double precision :: tol,ax,cx,gcv,xmin
        integer,intent(in) :: order
        type(bspline) :: f_sp

!        open(10,file="datapb.txt")
!        write(10,*) ti,yi
!        close(10)

        tol=1d-5!tolerance for golden search
        cx=1d10
        ax=1d-9
        gcv=brent(ax,cx,func,tol,xmin)
        !print *,'smoothing param',xmin
        !print *,'GCV value',gcv
        call spaps(ti,yi,-xmin,order=order,f_sp=f_sp,gcv=gcv)
        !print *,'GCV value',gcv
        contains
                function func(tol) result(gcv)
                        double precision,intent(in) :: tol
                        double precision :: gcv 
                        gcv=0d0
                        call spaps(ti,yi,-tol,order=order,f_sp=f_sp,gcv=gcv)
                end function func
end subroutine autospaps
subroutine spaps(ti,yi,tol,wi,order,f_sp,gcv) 
        double precision,dimension(:),intent(in) :: ti,yi
        double precision,dimension(:),allocatable :: t,y
        double precision,dimension(:),pointer,optional :: wi
        double precision :: tol ! 
        double precision,optional :: gcv ! 
        double precision,dimension(:,:),allocatable :: a,c,ww,cwc,mat_tmp
        type(mat_aij) :: mattmp,mat_c
        double precision,dimension(:),allocatable,target :: rhs,u,afp,w,tmp,knots,knotavg,aq,dt
        double precision, dimension(:,:),pointer :: ptu,ptc
        double precision :: p,E,fp,pn_1,fpn_1,eps,tolred,t1,t2
        integer,dimension(:),allocatable :: ipiv
        integer :: n,order,i,info
        type(bspline) :: df_sp,f_sp,sp
        logical :: integrate

        n=size(ti)
        allocate(t(n),source=ti)
        allocate(y(n),source=yi)
        if (present(wi)) then
              allocate(w(n),source=wi)
        else
                allocate(w(0))
        end if
        call chckxywp(t,y,max(2,order),w,tol,tolred) 
        !print *,'tol',tol,tolred
        if (tol>=0d0) then
                tol=tol-tolred
                if (tol<0d0) tol=1d-10
        end if
        n=size(t)
        allocate(rhs(n-order),u(n-order),afp(n),tmp(n-order)) 
        allocate(knots(n),cwc(n-order,n-order))
        
        call Cmatrix(t,order,c)
        call bsplmatrix(t,order,a)
!
!
        integrate=.true.
        if (tol <0 ) then
                allocate(mat_tmp(n-order,n))
                p=-tol
                !print *,'not supported now'
                mat_tmp=transpose(c)
                call convert_full_to_aij(transpose(c),mattmp)
                do i=1,n-order
                        c(:,i) = 1d0/w*c(:,i) !matmul(ww,c)
                end do
                call convert_full_to_aij(c,mat_c)
                call matmul_aij(mattmp,y,rhs)
                call matmul_aijm(mattmp,c,cwc)
                !print *,'c',size(c,1),size(c,2)
                !write(*,'(17(f10.4))') mat_tmp
                !write(*,'(17(f10.4))') transpose(a) 
                a=cwc+p*a
                !print *,'sz a',size(a,1),size(a,2)
                !print *,'rhs',rhs
                u=0d0
                !print *,'mat',transpose(a)
                !call directlu(u,a,rhs,.false.,ipiv)
                u=rhs
                call lutridiagblock(u,a,.false.,order) 
                !print *,'u',u
                !call dgetrs('N',n-order,n,a,n-order,ipiv,mat_tmp,n-order,info)
                call lutridiagblock(mat_tmp(:,i),a,.true.,order,mat_tmp) 
                
                !call invfact(a,ipiv)

                tmp=matmul(cwc,u)
                ptu(1:1,1:n-order) => u
                ptc(1:n-order,1:1) => tmp
                E=trace(matmul(ptu,ptc))+tolred
                deallocate(cwc)
                allocate(cwc(n,n))
                !cwc=matmul(c,mat_tmp,)
                call matmul_aijm(mat_c,mat_tmp,cwc)
                !print *,'cwc E tmp matrix',t2-t1 
                !Ici cest -, mais au carre =pareil cwc=-cwc
                fp=(trace(cwc))**2
                gcv=1d0/n*(E/fp)
                !print *,'E,fp',E,fp,norm(matmul(cwc,y))**2
                !print *,'gcv',gcv,p
                call matmul_aij(mat_c,u,afp)
                afp=y-afp
                !print *,'afp',norm(afp)
                !print *,'afp',afp


        else
                allocate(ww(n,n))
                ww=0d0
                do i=1,n
                        ww(i,i) = 1d0/w(i)
                end do
                allocate(mat_tmp(n-order,n-order))
                rhs=matmul(transpose(c),y)
                cwc=matmul(transpose(c),matmul(ww,c))
                mat_tmp=cwc
                !print *,'rhs',rhs
                !print *,'mat',transpose(a)
                u=0d0
                call directlu(u,mat_tmp,rhs,.false.)
                tmp=matmul(cwc,u)
                ptu(1:1,1:n-order) => u
                ptc(1:n-order,1:1) => tmp
                E=trace(matmul(ptu,ptc))
                               ! print *,'E',E
                p=0d0
                if (E<tol) then
                        afp=y-matmul(ww,matmul(c,u))
                        !print *,'afp e<tol',afp
                        integrate=.false.
                        p=0
                        allocate(f_sp%coefs(order),f_sp%knots(2*order))
                        f_sp%coefs(1:order)=afp(1)
                        f_sp%knots(1:order)=t(1)
                        f_sp%knots(order+1:2*order)=t(n)
                        f_sp%order=order
                        !construire la spline en augmentant les knots
                else
                !do a first step of newton then secant method
                        fp=1d0/sqrt(E)-1d0/sqrt(tol)
                        tmp=matmul(a,u)
                        !ptc(1:n-order,1:1) => tmp keep it comment for record
                        p=p-E*sqrt(E)*fp*1d0/trace(matmul(ptu,ptc))
                        eps=p
                        pn_1=0d0
                        fpn_1=fp
                        do while (abs(eps)>1d-13)
                                mat_tmp=cwc+p*a
                                u=0d0
                                call directlu(u,mat_tmp,rhs,.false.)
                                tmp=matmul(cwc,u)
                                !ptu(1:1,1:n-order) => u
                                !ptc(1:n-order,1:1) => tmp  keep it comment for record
                                E=trace(matmul(ptu,ptc))
                                !print *,'E',E
                                fp=1d0/sqrt(E)-1d0/sqrt(tol)
                                eps=-(eps)*1d0/(fp-fpn_1)*fp
                                if ((fp-fpn_1) .eq. 0d0) eps=1d-15
                                !if (eps.eq. huge(1d0)+huge(1d0)) exit 
                                if (eps.eq. huge(1d0)) exit 
                                p=p+eps!(p-pn_1)/(fp-fpn_1)*fp
                                fpn_1=fp
                        end do
                       afp=y-matmul(ww,matmul(c,u))
                       !print *,'p',p
                       !print *,'afp',afp
                end if
                deallocate(ww)
        end if
        if (integrate) then
                !Got the 2nd derivative now integrate to the final spline
                knots=t 
                u=p*u ! get the acoefs of the k-derivate bspline 
                df_sp%order=order
                allocate(df_sp%coefs(size(u,1)),df_sp%knots(size(t,1)))
                df_sp%knots=t
                df_sp%coefs=u

                !print *,'df_sp',df_sp%order
                !print *,'knots',df_sp%knots
                !print *,'coefs',df_sp%coefs
                f_sp=df_sp
                do i=1,order-1
                !print *,'integration'
                f_sp=integrate_bspcoef(f_sp)
                !call derivate_bsp(f_sp%coefs,f_sp%knots,f_sp%order)
                !stop
                end do
                !print *,'integration'
                f_sp=integrate_bspcoef(f_sp,afp(1))
                deallocate(df_sp%coefs,df_sp%knots)
        end if

        !print *,'f_sp', f_sp%order
        !print *,'knots',f_sp%knots
        !print *,'coefs',f_sp%coefs

        !call derivate_bsp(u,knots,order)
        call avknot(f_sp%knots,f_sp%order,knotavg)
        !print *,'avknot',knotavg
        !print *,bsp_eval(f_sp,6.28d0)
        allocate(aq(size(knotavg,1)))
        aq=0d0
        aq=polyval(polyfit(t-t(1),afp-bsp_evalv(f_sp,t),order-1),knotavg-t(1))
        !print *,'aq',aq
        call bsplint(knotavg,aq,f_sp%knots,sp)
        f_sp%coefs=f_sp%coefs+sp%coefs
        !print  *,'coefs final',f_sp%coefs
        !print  *,'coefs knots',f_sp%knots
        !print  *,'coefs order',f_sp%order
        
        deallocate(rhs,u,afp,tmp,a,c,sp%coefs,sp%knots) 
        deallocate(knots,w,mat_tmp,cwc,aq,knotavg)


        contains
                function trace(mat)
                        double precision,dimension(:,:),intent(in)  :: mat
                        double precision :: trace
                        integer :: i
                        trace=0d0
                        do i=1,size(mat,1)
                                trace=trace+mat(i,i)
                        end do
                end function
        
end subroutine spaps

subroutine derivate_bsp(acoefs,t,k)!bcoefs,tt)
        double precision,dimension(:),intent(in) :: acoefs,t
        double precision,dimension(:),allocatable :: bcoefs,tt,difftk,diffac
        integer ,dimension(:),allocatable :: indx
        integer :: n,nt,m,nn,k,i

        nt=size(t,1)
        n=size(acoefs)
        print *,' STILL in test '

        allocate(diffac(n+1),difftk(nt-k+1))
        !print *,'k',k
        difftk=t(1+k-1:n+k)-t(1:n+1)
        print *,'difftk',difftk
        diffac(2:n)=acoefs(2:n)-acoefs(1:n-1)
        diffac(1)=acoefs(1)
        diffac(n+1)=-acoefs(n)
        !print *,'diffac',diffac
        m=count(difftk>0)
        !print *,'m',m
        allocate(indx(m))
        indx=pack([(i,i=1,size(difftk))],difftk>0)
        !print *,'indx',indx

        allocate(bcoefs(m),tt(m+k-1))

        bcoefs=diffac(indx)*(k-1)*1d0/difftk(indx)

        !print *,'bcoefs',bcoefs
        tt(1:m)=t(indx)
        tt(m+1:m+k-1)=t(n+2:n+k)
        !print *,'tt',tt
         
end subroutine derivate_bsp

function integrate_bspcoef(sp,first_val) result(isp)
        type(bspline) :: sp
        type(bspline) :: isp
        double precision,intent(in),optional :: first_val
        double precision,dimension(:),pointer :: t,tmp
        double precision,dimension(:),pointer :: acoefs,bcoefs,btmp
        double precision,dimension(:),allocatable :: difft
        integer,dimension(:),allocatable :: ind
        integer :: n,nt,k,i,nbz
        
        allocate(acoefs(size(sp%coefs,1)),t(size(sp%knots,1)))
       
       !!!! USE move_alloc f2003 

        acoefs=sp%coefs
        t=sp%knots
        n=size(acoefs,1)
        nt=size(t,1)
        k=sp%order
        !allocate(tmp(nt))
        tmp=>t
        !deallocate(t)
                !Complete coeffs with 0d0 values at end
        !allocate(tmp(n))
        !tmp=acoefs
        !deallocate(acoefs)
                
        !t=[t t(end)]
        !increase last knot multiplicity k
        allocate(difft(nt-1))
        difft=diff(t)
        allocate(ind(count(difft>0)))
        ind=pack([(i,i=1,nt-1)],difft>0)
        !print *,'difft',size(ind,1),difft
        !print *,'ind',size(ind,1),ind
        nbz=ind(size(ind,1))-(nt-k-1)
        !print *,'last nbz',nbz
        deallocate(difft,ind)

        allocate(t(nt+nbz))

        t(1:nt)=tmp
        t(nt+1:nt+nbz)=t(nt)
        deallocate(tmp) 
        allocate(bcoefs(nt+nbz-k-1))
        bcoefs=0d0

        bcoefs(1)=acoefs(1)*(t(k+1)-t(1))*1d0/k;
        do i=2,n
                bcoefs(i)=bcoefs(i-1)+acoefs(i)*(t(k+i)-t(i))*1d0/k;
        end do
        bcoefs(n+1:nt+nbz-k-1)=bcoefs(n)
        nt=nt+nbz
        !print *,'t',t
        if (present(first_val)) then
                allocate(difft(nt))
                difft=diff(t)
                allocate(ind(count(difft>0)))
                ind=pack([(i,i=1,nt-1)],difft>0)
                nbz=k+1-ind(1)
                !print *,'begin',nbz
               !inutile if (nbz>0)
                 allocate(tmp(nt+nbz), btmp(n+nbz))
                
                do i=1,nbz
                        tmp(i)=t(1)
                        !t=[t(1) t]
                        btmp(i)=0d0
                end do
                tmp(nbz+1:nt+nbz)=t
                btmp(nbz+1:n+nbz)=bcoefs
                btmp=btmp+first_val
                deallocate(t,bcoefs)
                t=>tmp
                bcoefs=>btmp
                deallocate(ind,difft)

        end if

        !print *,'t',t
        !print *,'acoefs',bcoefs
        !deallocate(sp%knots,sp%coefs)
        allocate(isp%coefs(size(bcoefs,1)),isp%knots(size(t,1)))
        isp%order=k+1
        isp%knots=t
        isp%coefs=bcoefs
        deallocate(acoefs,t,bcoefs)
end function integrate_bspcoef

subroutine bsplint(x,y,t,sp)
        double precision,dimension(:),intent(in) :: x,y,t
        double precision,dimension(:),allocatable :: asp
        type(bspline) :: sp
        integer :: order,i,j,n,m
        double precision,dimension(:,:),allocatable :: bmat

        
        n=size(y,1)
        order=size(t,1)-n
        !print *,'order',order,n
        allocate(bmat(n,n),asp(n))

        !construct of the "coloc matrix" 
        do i=1,n
                do j=1,n
                        bmat(i,j)=bsplrr(j,order,t,x(i))
                        !to be optimized
                end do
        end do
        if(bmat(1,1)==0d0) bmat(1,1)=1d0 ! trick left condition...
        !print *,'right continuous condition to be fixed'
        bmat(n,n)=1d0
       
        !print *,'mat',transpose(bmat)
        asp=0d0
        call directlu(asp,bmat,y,.false.)

        !print *,'asp',asp
        allocate(sp%knots(n+order))
        allocate(sp%coefs(n))
        sp%coefs=asp
        sp%knots=t
        sp%order=order
        deallocate(asp,bmat)

end subroutine bsplint


subroutine csaps(t,y,p,wi)
        double precision,dimension(:),intent(in) :: t,y
        double precision,dimension(:),allocatable,optional :: wi
        double precision,optional :: p ! 0<p<1
        double precision,dimension(:,:),allocatable :: a,c,ww
        double precision,dimension(:),allocatable :: rhs,u,afp,w
        integer :: n,order,i

        n=size(t)
        order=2
        
        allocate(rhs(n-order),u(n-order),afp(n)) 
        allocate(ww(n,n),w(n))
        w=1d0
        ww=0d0
        if (present(wi)) then
              w=wi
        end if 
        call Cmatrix(t,order,c)
        call bsplmatrix(t,order,a)
        !print *,'mat',transpose(a)
        stop
        do i=1,n
                ww(i,i) = 1d0/w(i)
        end do
        rhs=matmul(transpose(c),y)
        a=matmul(transpose(c),matmul(ww,c))+p*a
        !print *,'rhs',rhs
        !print *,'mat',transpose(a)
        u=0d0
        call directlu(u,a,rhs,.false.)
        
        !print *,'u',u
        afp=y-1d0/p*matmul(ww,matmul(c,u))
        
        !print *,'afp',afp
 ! do the ppform of the cubic smooth spline with p the smooth parameter.        
end subroutine csaps

function polyval(p,x) result(y)
        double precision,dimension(:),intent(in) :: p,x
        double precision,dimension(:),allocatable :: y
        integer :: i,np,nx
        nx=size(x,1)
        np=size(p,1)
        allocate(y(nx))

        ! Use Horner's method for general case where X is an array.
        y = 0d0 !zeros(siz_x, superiorfloat(x,p))
        if (np>0) y = p(1)
        do i=2,np
            y = x*y + p(i)
        end do



end function polyval
function polyfit(x,y,degree) result(p)
        double precision,dimension(:),intent(in) :: x,y
        integer,intent(in) :: degree
        double precision,dimension(:,:),allocatable :: V
        double precision,dimension(:),allocatable :: p
        integer :: j
        
        !construct vandermonde matrix
        allocate(V(size(x,1),degree+1))
        V(:,degree+1)=1d0
        do j=degree,1,-1
                V(:,j)=x*V(:,j+1)
        end do
        
        allocate(p(degree+1))
        p=0d0
        !Solve  Least Square
        call directlu(p,V,y,.false.)
        !print *,'polyfit p',p
        deallocate(V)

end function

subroutine avknot(x,k,y)
double precision,dimension(:),intent(in) :: x
integer, intent(in) :: k !order
integer :: i
double precision,dimension(:),allocatable :: y

allocate(y(size(x,1)-k))
do i=1,size(x,1)-k
        y(i)=sum(x(i+1:i+k-1))*1d0/(k-1)
end do
end subroutine


subroutine Cmatrix(t,order,c)
        double precision,dimension(:),intent(in) :: t
        double precision,dimension(:,:),allocatable :: C
        integer,intent(in) :: order
        integer :: n,j,k
        n=size(t)
        allocate(C(n,n-order))
        c=0d0
        do k=1,n-order
        do j=k,k+order
                c(j,k)=(t(k+order)-t(k))*1d0/prod(j)
               ! print *,j,k,c(j,k)
        end do
        end do

        C=factorial(order-1)*C

contains
        function prod(j)
                integer,intent(in)  :: j
                double precision :: prod
                integer :: i
                prod=1d0
                do i=k,k+order
                        if (j .ne. i) prod=prod*(t(j)-t(i))
                end do

        end function
        recursive function factorial(n)  result(fact)

                IMPLICIT NONE
                INTEGER :: Fact
                INTEGER, INTENT(IN) :: n

                IF (n == 0) THEN
                        Fact = 1
                ELSE
                        Fact = n * Factorial(n-1)
                END IF

        end function factorial

end subroutine cmatrix
subroutine bsplmatrix(t,order,a,lambda)
        integer,intent(in) :: order
        double precision,dimension(:),intent(in) :: t
        double precision,dimension(:),intent(in),optional :: lambda
        integer :: i,j,n,m
        double precision,dimension(:,:),allocatable :: a
        n=size(t)
        m=n-order
        allocate(a(m,m))
        a=0d0
        !print *,'order',order
        do j=1,m
                !do i=1,m
                do i=1+posp(j-3),j
                        a(i,j)=ajk(i,j,t,order)
                        !print *,i,j,a(i,j)
                        a(j,i)=a(i,j)
                end do
        end do
        contains
function posp(x)
        integer :: x,posp
        posp=x
        if (x<0) posp=0
end function
end subroutine


function ajk(j,k,t,order)
        integer,intent(in) :: j,k 
        double precision :: ajk
        integer ::a,b,i,order
        double precision,dimension(:) :: t
        a=min(j,k)
        b=max(j+order,k+order)
        ajk=0d0
        do i=a,b-1
                !print *,'ta,tb',t(i),t(i+1)
                ajk=ajk+qgauss(bsplfrr,t(i),t(i+1))
        end do


contains
function bsplfrr(x) result(y)
        double precision,dimension(:),intent(in) :: x
        double precision,dimension(size(x)) :: y
        integer :: i
        do i=1,size(x)
                y(i)= bsplrr(j,order,t,x(i))* bsplrr(k,order,t,x(i))
        end do
end function
end function


function bsp_evalv(sp,x) result(y)
        type(bspline),intent(in) :: sp
        double precision,dimension(:),intent(in) :: x
        double precision,dimension(:),allocatable :: y
        integer :: i
        allocate(y(size(x,1)))

        do i=1,size(x,1)
                y(i)=bsp_evalbv(sp,x(i))
        end do

        
end function

function bsp_evalbv(sp,x) result(res)
        type(bspline),intent(in) :: sp
        double precision,intent(in) :: x
        double precision :: res

        res=bvalue(sp%knots,sp%coefs,size(sp%coefs), sp%order, x, 0 )
end function bsp_evalbv 
function bsp_evald(sp,x) result(res)
        type(bspline),intent(in) :: sp
        double precision,intent(in) :: x
        double precision,dimension(2) :: res

        res(1)=bvalue(sp%knots,sp%coefs,size(sp%coefs), sp%order, x, 0 )
        res(2)=bvalue(sp%knots,sp%coefs,size(sp%coefs), sp%order, x, 1 )
end function bsp_evald 
subroutine interv ( xt, lxt, x, left, mflag )

!*****************************************************************************80
!
!! INTERV brackets a real value in an ascending vector of values.

  implicit none

  integer ( kind = 4 ) lxt

  integer ( kind = 4 ) left
  integer ( kind = 4 ) mflag
  integer ( kind = 4 ) ihi
  integer ( kind = 4 ), save :: ilo = 1
  integer ( kind = 4 ) istep
  integer ( kind = 4 ) middle
  real ( kind = 8 ) x
  real ( kind = 8 ) xt(lxt)

  ihi = ilo + 1

  if ( lxt <= ihi ) then

    if ( xt(lxt) <= x ) then
      go to 110
    end if

    if ( lxt <= 1 ) then
      mflag = -1
      left = 1
      return
    end if

    ilo = lxt - 1
    ihi = lxt

  end if

  if ( xt(ihi) <= x ) then
    go to 20
  end if

  if ( xt(ilo) <= x ) then
    mflag = 0
    left = ilo
    return
  end if
!
!  Now X < XT(ILO).  Decrease ILO to capture X.
!
  istep = 1

10 continue

  ihi = ilo
  ilo = ihi - istep

  if ( 1 < ilo ) then
    if ( xt(ilo) <= x ) then
      go to 50
    end if
    istep = istep * 2
    go to 10
  end if

  ilo = 1

  if ( x < xt(1) ) then
    mflag = -1
    left = 1
    return
  end if

  go to 50
!
!  Now XT(IHI) <= X.  Increase IHI to capture X.
!
20 continue

  istep = 1

30 continue

  ilo = ihi
  ihi = ilo + istep

  if ( ihi < lxt ) then

    if ( x < xt(ihi) ) then
      go to 50
    end if

    istep = istep * 2
    go to 30

  end if

  if ( xt(lxt) <= x ) then
    go to 110
  end if
!
!  Now XT(ILO) < = X < XT(IHI).  Narrow the interval.
!
  ihi = lxt

50 continue

  do

    middle = ( ilo + ihi ) / 2

    if ( middle == ilo ) then
      mflag = 0
      left = ilo
      return
    end if
!
!  It is assumed that MIDDLE = ILO in case IHI = ILO+1.
!
    if ( xt(middle) <= x ) then
      ilo = middle
    else
      ihi = middle
    end if

  end do
!
!  Set output and return.
!
110 continue

  mflag = 1

  if ( x == xt(lxt) ) then
    mflag = 0
  end if

  do left = lxt, 1, -1
    if ( xt(left) < xt(lxt) ) then
      return
    end if
  end do

  return
end subroutine 
function bvalue ( t, bcoef, n, k, x, jderiv )

!*****************************************************************************80
!
!! BVALUE evaluates a derivative of a spline from its B-spline representation.  

  implicit none

  integer ( kind = 4 ) k
  integer ( kind = 4 ) n

  real ( kind = 8 ) aj(k)
  real ( kind = 8 ),intent(in) :: bcoef(n)
  real ( kind = 8 ) bvalue
  real ( kind = 8 ) dl(k)
  real ( kind = 8 ) dr(k)
  integer ( kind = 4 ) i
  integer ( kind = 4 ) ilo
  integer ( kind = 4 ) j
  integer ( kind = 4 ) jc
  integer ( kind = 4 ) jcmax
  integer ( kind = 4 ) jcmin
  integer ( kind = 4 ) jderiv
  integer ( kind = 4 ) jj
  integer ( kind = 4 ) mflag
  real ( kind = 8 ),intent(in) ::t(n+k)
  real ( kind = 8 ) x

  bvalue = 0.0D+00
  
  if ( k <= jderiv ) then
    return
  end if
!
!  Find I so that 1 <= I < N+K and T(I) < T(I+1) and T(I) <= X < T(I+1). 
!
!  If no such I can be found, X lies outside the support of the 
!  spline F and  BVALUE = 0.  The asymmetry in this choice of I makes F 
!  right continuous.
!
  call interv ( t, n+k, x, i, mflag )
  
  if ( mflag /= 0 ) then
    return
  end if
!
!  If K = 1 (and JDERIV = 0), BVALUE = BCOEF(I).
!
  if ( k <= 1 ) then
    bvalue = bcoef(i)
    return
  end if
!
!  Store the K B-spline coefficients relevant for the knot interval
!  ( T(I),T(I+1) ) in AJ(1),...,AJ(K) and compute DL(J) = X - T(I+1-J),
!  DR(J) = T(I+J)-X, J=1,...,K-1.  Set any of the AJ not obtainable
!  from input to zero.  
!
!  Set any T's not obtainable equal to T(1) or to T(N+K) appropriately.
!
  jcmin = 1
  
  if ( k <= i ) then
  
    do j = 1, k-1
      dl(j) = x - t(i+1-j)
    end do
    
  else
  
    jcmin = 1 - ( i - k )
 
    do j = 1, i
      dl(j) = x - t(i+1-j)
    end do
 
    do j = i, k-1
      aj(k-j) = 0.0D+00
      dl(j) = dl(i)
    end do
  
  end if
 
  jcmax = k

  if ( n < i ) then
 
    jcmax = k + n - i
    do j = 1, k + n - i
      dr(j) = t(i+j) - x
    end do
 
    do j = k+n-i, k-1
      aj(j+1) = 0.0D+00
      dr(j) = dr(k+n-i)
    end do
 
  else
 
    do j = 1, k-1
      dr(j) = t(i+j) - x
    end do
 
  end if
 
  do jc = jcmin, jcmax
    aj(jc) = bcoef(i-k+jc)
  end do
!
!  Difference the coefficients JDERIV times.
!
  do j = 1, jderiv
 
    ilo = k - j
    do jj = 1, k - j
      aj(jj) = ( ( aj(jj+1) - aj(jj) ) / ( dl(ilo) + dr(jj) ) ) &
        * real ( k - j, kind = 8 )
      ilo = ilo - 1
    end do
 
  end do
!
!  Compute value at X in (T(I),T(I+1)) of JDERIV-th derivative,
!  given its relevant B-spline coefficients in AJ(1),...,AJ(K-JDERIV).
!
  do j = jderiv+1, k-1
    ilo = k-j
    do jj = 1, k-j
      aj(jj) = ( aj(jj+1) * dl(ilo) + aj(jj) * dr(jj) ) &
        / ( dl(ilo) + dr(jj) )
      ilo = ilo - 1
    end do
  end do
  
  bvalue = aj(1)
 
  return
end function bvalue
function bsp_eval(sp,x,mi) result(y)
        type(bspline),intent(in) :: sp
        double precision,intent(in) :: x
        double precision :: y,bjk
        integer :: i,j,k,m,szknt,mxnbbasis
        integer ,optional :: mi

        y=0d0
        if (.not. present(mi)) then 
                m=0
        else
                m=mi
        end if
        szknt=size(sp%knots,1)
        if (x > sp%knots(szknt)) return 
        k=sp%order
        mxnbbasis=szknt-k

        j=-1
        do i=1,szknt
                if(x<sp%knots(i)) then
                        j=i-1
                        exit
                end if
        end do
        if (j.eq.-1) then
                j= szknt-1!size(sp%knots,1)-1
                do while (sp%knots(j) .eq. sp%knots(j+1))
                        j= j-1
                end do 
                j=min(j,mxnbbasis)
                !print *,'right continuous condition'
                if(count(sp%knots==sp%knots(j+1))==k) then
                        bjk=1d0
                        y=sp%coefs(j)*bjk
                        !print *,'value of sp at x',y
                        !return
                end if

        end if
        !print *,'interval',sp%knots
        !print *,'interval',j,j+1
        !print *,'bspline nb',max(1,j+1-k),'to',min(j,mxnbbasis)
        !print *,'value of sp at x',x,y

        if (m>0) then
                !y=0d0
                do i=max(1+m,j+1-k+m),min(j+m,mxnbbasis+m)
                        !print *,'im',1+m,j,mxnbbasis+m
                        if(i>mxnbbasis) then
                                y=y+(k-m)*(-sp%coefs(i-1))/(sp%knots(i+k-m)-sp%knots(i))*bsplrr(i,k-m,sp%knots,x)
                        else
                                y=y+(k-m)*(sp%coefs(i)-sp%coefs(i-1))/(sp%knots(i+k-m)-sp%knots(i))*bsplrr(i,k-m,sp%knots,x)
                        end if
                        !print *,'value of sp',i,y,bsplrr(i,k-m,sp%knots,x)
                end do
        else

                do i=max(1,j+1-k),min(j,mxnbbasis)
                        y=y+sp%coefs(i)*bsplrr(i,k,sp%knots,x)
                        !print *,'value of sp',i,y,bsplrr(i,k,sp%knots,x)
                end do
                !print *,'value of sp at x',y
        end if

        !print *,'value of sp at x',x,y

end function

recursive function bsplrr (j,k,t,x) result(bjk)
        integer,intent(in) :: j,k
        double precision,intent(in) :: x
        double precision,dimension(:),intent(in) :: t
        double precision :: bjk
        !k here is order, see definition by PGS !
        if( (t(j) .le. x ).and. (x .lt. t(j+k) )) then 
                !print *,'tj,x,tj+1',t(j),x,t(j+1)
                if (k.eq.1) then
                        bjk=1d0
                else
                        if (t(j+1) .eq. t(j+k)) then
                                !print *,'knot multiple back',x,t(j), t(j+k)
                                bjk=((x-t(j))*1d0/(t(j+k-1)-t(j)))**(k-1)
                                !print *,'bjk',bjk,j,k

                        else if (t(j) .eq. t(j+k-1)) then 

                                !print *,'knot multiple front',x,t(j), t(j+k)
                                bjk=( (t(j+k)-x)*1d0/(t(j+k)-t(j+1)) )**(k-1)
                                !print *,'bjk',bjk,j,k
                        else
                                !print *,'bjk needs bjk-1,bj+1k-1',j,k!,bjk
                                !print *,'tj,tj+1,tj+k',t(j),t(j+1),t(j+k),t(j+k-1)
                                bjk=((x-t(j))*1d0/(t(j+k-1)-t(j)))*bsplrr(j,k-1,t,x) +&
                                    ((t(j+k)-x)*1d0/(t(j+k)-t(j+1)))*bsplrr(j+1,k-1,t,x)
                        end if
                end if
        else
                bjk=0d0
        end if
        !print *,'bjk,j,k',bjk,j,k

end function

function qgauss(func,a,b)
double precision, intent(in) :: a,b
double precision :: qgauss
interface
        function func(x)
        double precision,dimension(:), intent(in) :: x
        double precision, dimension(size(x)) :: func
        end function func
end interface

double precision, dimension(8) ::  w,x
x=0d0
w=0d0
call gauleg(a,b,x,w)
qgauss=dot_product(w,func(x))
end function qgauss

subroutine gauleg(x1,x2,x,w)
implicit none
double precision, intent(in) :: x1,x2
double precision, dimension(:), intent(out) :: x,w
double precision, parameter :: eps=1.0e-14
!   given the lower and upper limits of integration x1 and x2, this routine returns arrays x and w
!   of length n containing the abscissas and weights of the gauss-legendre n -point quadrature
!   formula. the parameter eps is the relative precision. note that internal computations are
!   done in double precision.
integer :: its,j,m,n,i
integer, parameter :: maxit=10
double precision :: xl,xm
double precision, dimension((size(x)+1)/2) :: p1,p2,p3,pp,z,z1
double precision,dimension((size(x)+1)/2) :: temp
logical, dimension((size(x)+1)/2) :: unfinished
n=size(x,1)!,size(w),’gauleg’)
!                                                       the roots are symmetric in the interval,
m=(n+1)/2
 !                                                          so we only have to find half of them.
xm=0.5d0*(x2+x1)
xl=0.5d0*(x2-x1)
 !                                                          initial approximations to the roots.
do i=1,m
        temp(i)=i!(/(i,i=1,m)/)
end do
z=cos(pi*(temp-0.25d0)*1d0/(n+0.5d0))
unfinished=.true.
 !                                                      newton’s method carried out simultane-
do its=1,m
 !                                                          ously on the roots.
    where (unfinished)
         p1=1.0d0
         p2=0.0d0
    end where
 !                                                      loop up the recurrence relation to get
    do j=1,n
 !                                                          the legendre polynomials evaluated
         where (unfinished)
 !                                                          at z.
             p3=p2
             p2=p1
             p1=((2.0d0*j-1.0d0)*z*p2-(j-1.0d0)*p3)*1d0/j
         end where
    end do
 !     p1 now contains the desired legendre polynomials. we next compute pp, the derivatives,
 !     by a standard relation involving also p2, the polynomials of one lower order.
    where (unfinished)
         pp=n*(z*p1-p2)*1d0/(z*z-1.0d0)
         z1=z
  !                                                     newton’s method.
         z=z1-p1*1d0/pp
         unfinished=(abs(z-z1) > eps)
    end where
    if (.not. any(unfinished)) exit
end do
if (its == m+1) print *,'too many iterations in gauleg'
 !                                                      scale the root to the desired interval,
x(1:m)=xm-xl*z
 !                                                      and put in its symmetric counterpart.
x(n:n-m+1:-1)=xm+xl*z
  !                                                     compute the weight
w(1:m)=2.0d0*xl*1d0/((1.0d0-z**2)*pp**2)
   !                                                    and its symmetric counterpart.
w(n:n-m+1:-1)=w(1:m)
end subroutine gauleg


subroutine cubspl(n,t,coef,boundbeg,boundend)
integer,intent(in) :: n,boundbeg,boundend
double precision,dimension(:),intent(in) :: t
double precision,dimension(:,:) :: coef
double precision,dimension(:),allocatable :: dt,gdivdif,temp_dat,ld,dd,ud
double precision,dimension(:),allocatable,target :: b
double precision,dimension(:),pointer :: s
double precision :: tmp
integer :: i

allocate(dt(n-1),gdivdif(n-1),ld(n-1),dd(n),ud(n-1),b(n),temp_dat(n))

!coef(1,:)=data in each t
temp_dat=coef(1,:)
dt(:)=t(2:n)-t(1:n-1)
gdivdif(:)=(temp_dat(1:n-1)-temp_dat(2:n))/(-dt(:))


!compute s(i)
!set tridiag
ld(1:n-2)=dt(2:n-1)
ld(n-1)=dt(n-2)+dt(n-1)

ud(1)=dt(1)+dt(2)
ud(2:n-1)=dt(1:n-2)

dd(1)=dt(2)
dd(2:n-1)=2d0*(dt(1:n-2)+dt(2:n-1))
dd(n)=dt(n-2)


b(1)=((dt(1)+2d0*ud(1))*dt(2)*gdivdif(1)+dt(1)*dt(1)*gdivdif(2))/(ud(1))
b(2:n-1)=3d0*(dt(2:n-1)*gdivdif(1:n-2)+dt(1:n-2)*gdivdif(2:n-1))
b(n)=(dt(n-1)*dt(n-1)*gdivdif(n-2)+(2d0*ld(n-1)+dt(n-1))*dt(n-2)*gdivdif(n-1))/ld(n-1)


call tridiaglu(ld,ud,dd,b)
s=>b
coef(2,1:n-1)=b(1:n-1)

do i=1,n-1
        tmp=s(i)+s(i+1)-2d0*gdivdif(i)
        coef(3,i)=2d0*(gdivdif(i)-s(i)-tmp)*1d0/dt(i)
        coef(4,i)=6d0*tmp/(dt(i)*dt(i))
end do
!!print *,'n',n
!!print *,'s',s
end subroutine cubspl


subroutine tridiaglu(ld,ud,dd,b)
double precision,dimension(:) :: ld,dd,ud
double precision,dimension(:),target :: b
double precision,dimension(:),pointer :: x
integer :: i,n

n=size(dd,1)

do i=2,n
        ld(i-1)=ld(i-1)*1d0/dd(i-1)
        dd(i)=dd(i)-ld(i-1)*ud(i-1)
        b(i)=b(i)-ld(i-1)*b(i-1)
end do

x=>b
x(n)=b(n)/dd(n)

do i=n-1,1,-1
        x(i)=(b(i)-ud(i)*x(i+1))*1d0/dd(i)
end do


!verification

end subroutine tridiaglu


subroutine invfact(mat,ipiv)
double precision,dimension(:,:),intent(inout) :: mat
integer,dimension(:),intent(in) :: ipiv
double precision,dimension(:),allocatable :: work
integer :: n,lwork,info
n=size(mat,1)
lwork=n+1
allocate(work(lwork))

!call getrf(mat,ipiv)
!call getri(mat,ipiv)
call dgetri(n, mat, n, ipiv, work, lwork, info )
if (info<0) print *,'problem ',info

deallocate(work)

end subroutine invfact

subroutine directlu(x,mat,b,isfact,ipiv)
!use mkl95_lapack
double precision,dimension(:),intent(out) :: x
double precision,dimension(:),intent(in) :: b
double precision,dimension(:,:),intent(inout) :: mat 
double precision,dimension(:,:),allocatable :: mat2 
integer,dimension(:),allocatable,save :: pivot
integer,dimension(:),allocatable,optional :: ipiv
double precision,dimension(:),allocatable :: tmpb
logical,intent(in) :: isfact
integer :: n,m,i,info
double precision :: anorm,rcond
character(1) :: mode

n=size(mat,1)
m=size(mat,2)
allocate(tmpb(size(b,1)))
!mat2=0d0
mode='N'
tmpb=b
!!print *,'mat',n,m
!!print *,'b',size(tmpb,1)
!!print *,'piv siz',size(pivot)
!pause
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
        if(present(ipiv)) then
                allocate(ipiv(size(pivot)))
                ipiv=pivot
        end if
else
        call dgetrs(mode,m,1,mat,n,pivot,tmpb,n,info)
        !print *,'solved',info
end if
x=tmpb(1:m)
!print *,"norm directlu ",norm(b-matmul(mat2,x))
deallocate(tmpb)
end subroutine directlu

subroutine inv(mat)
double precision,dimension(:,:),intent(inout) :: mat
integer,dimension(:),allocatable :: ipiv
double precision,dimension(:),allocatable :: work
integer :: n,m,lwork,info
n=size(mat,1)
m=size(mat,2)
lwork=n+1
allocate(work(lwork),ipiv(n))

!call getrf(mat,ipiv)
call dgetrf(n,m,mat,n,ipiv,info)
if (info<0) print *,'problem ',info
!call getri(mat,ipiv)
call dgetri(n, mat, n, ipiv, work, lwork, info )
if (info<0) print *,'problem ',info

deallocate(work,ipiv)

end subroutine inv

subroutine lutridiagblock(x2,mat2,isfact,m,xmat2)
!use mkl95_lapack
double precision,dimension(:),intent(inout),target :: x2
double precision,dimension(:),allocatable,target :: x
double precision,dimension(:,:),intent(inout),target :: mat2 
double precision,dimension(:,:),optional :: xmat2
logical,intent(in) :: isfact
integer,intent(in) :: m
double precision,dimension(:,:),pointer :: d,y
double precision,dimension(:,:),pointer :: temp
double precision,allocatable,dimension(:,:),target:: mat,xmat
double precision,allocatable,dimension(:,:),save :: U,L
double precision,allocatable,dimension(:,:),save :: dd
integer :: n,k,j,nrhs
double precision :: t1,t2
! PAS VECTORIEL POUR L'INSTANT !! 
n=size(mat2,1)
if (mod(size(mat2,1),m) .ne. 0) then
allocate(mat(n+1,n+1),x(n+1))
mat(1:n,1:n)=mat2
mat(n+1,n+1)=1d0
!x(1:n)=x2
!x(n+1)=0d0
else
        allocate(mat(n,n),source=mat2)
        allocate(x(n),source=x2)
end if
if(present(xmat2)) then
        nrhs=size(xmat2,2)
        allocate(xmat(n+1,nrhs))
        xmat(1:n,1:nrhs)=xmat2
        xmat(n+1,1:nrhs)=0d0
else
        nrhs=1
        allocate(xmat(n+1,nrhs))
        xmat(1:n,1)=x2
        xmat(n+1,1)=0d0
end if
n=size(mat,1)/m
!print *,'m',m,'n',n,size(mat,1)
if (.not. isfact) then 
        if( allocated(U)) then 
                deallocate(U,L)
        end if
        allocate(U(1:m,1:n*m))
        allocate(L(1:m,1:(n)*m))
        L=0d0
        U=0d0
        !c=0d0
        U(1:m,1:m)=mat(1:m,1:m)
        call inv(U(1:m,1:m))

        do k=2,n-1
            j=(k-1)*m+1
            L(1:m,j:k*m)=matmul(mat(j:k*m,(k-1-1)*m+1:(k-1)*m),U(1:m,(k-1-1)*m+1:(k-1)*m)) !produit matmat
            U(1:m,j:k*m)=mat(j:k*m,j:k*m)-matmul(L(1:m,j:k*m),mat((k-1-1)*m+1:(k-1)*m,j:k*m)) ! produit matmat
            call inv(U(1:m,j:k*m))
        end do 

        k=n
        j=(k-1)*m+1
        !normal version
        L(1:m,j:k*m)=matmul(mat(j:k*m,(n-2)*m+1:(n-1)*m),U(1:m,(n-2)*m+1:(n-1)*m)) !produit matmat

        !normal version   
        U(1:m,j:k*m)=mat(j:k*m,j:k*m)-matmul(L(1:m,(n-1)*m+1:(n)*m),mat((n-2)*m+1:(n-1)*m,j:k*m))!produit matmat
        call inv(U(1:m,j:k*m))

end if
!print *,'L'
!write(*,'(2(f18.10))') L
!print *,'U'
!write(*,'(2(f18.10))') U
!print *,'c'
!write(*,'(2(f18.10))') c
allocate(dd(n*m,nrhs))
dd=0d0
d => xmat!(:,1)
y => xmat!(:,1)
dd(1:m,:)=d(1:m,:);
do k=2,n-1
    j=(k-1)*m+1;
    dd(j:k*m,:)=d(j:k*m,:)-matmul(L(1:m,j:k*m),dd((k-2)*m+1:(k-1)*m,:)); !produit matvect
end do


dd((n-1)*m+1:n*m,:)=d((n-1)*m+1:n*m,:)-matmul(L(1:m,(n-1)*m+1:n*m),dd((n-2)*m+1:(n-1)*m,:)) ! produit matvect normal version



y((n-1)*m+1:n*m,:)=matmul(U(1:m,(n-1)*m+1:n*m),dd((n-1)*m+1:n*m,:)); ! produit matvect

do k=n-1,1,-1
    j=(k-1)*m+1;
    y(j:k*m,:)=matmul(U(1:m,j:k*m),(dd(j:k*m,:)-matmul(mat((k-1)*m+1:k*m,k*m+1:(k+1)*m),y(k*m+1:(k+1)*m,:)))); ! Attention au .* !!
enddo
!print *,'y'
!write(*,'(1(f18.10))') y
if (present(xmat2)) then
        xmat2=y(1:n*m-1,:) ! pas forcement corrcet depend de la taille de bloc et du nombre
        deallocate(xmat)
else
        x2=y(1:n*m-1,1) ! pas forcement corrcet depend de la taille de bloc et du nombre
!de point a rajouter
end if
deallocate(dd,mat,x)
end subroutine lutridiagblock




function norm(vect)
        double precision,dimension(:),intent(in) :: vect
        double precision :: norm
        norm=0d0
        norm=sqrt(dot_product(vect,vect))
end function norm


!![knots,coefs,k,sizec] 
!function  chckknt(knots,coefs,sizec,k)
!        double precision ,dimension(:) :: knots,coefs
!integer :: sizec,n,k
!!CHCKKNT check knots, omit trivial B-splines
!
!sizec=1
!
!!do j=1:size(sizec)-1
!   n = sizec(j+1)
!   k = size(knots)-n
!   if (k<=0) print *,'SPLINES:SPMAK:knotsdontmatchcoefs'
!   if (any(diff(knots)<0))
!      print *,'SPLINES:SPMAK:knotdecreasing'
!   end if
!   if (knots(1)==knots(size(knots))
!      print *,'SPLINES:SPMAK:extremeknotssame'
!   end if
!
!   ! make sure knot sequence is a row matrix:
!   !knots{j} = reshape(knots{j},1,n+k(j));
!
!   ! throw out trivial B-splines:  pas besoin de sa ,a revoir si pb !
!   !index = pack([1],knots(k+[1:n])-knots([1:n])>0);
!
!   !if (size(index)<n)
!   !   oldn = n; n = length(index);
!   !   knots{j} = reshape(knots{j}([index oldn+(1:k(j))]),1,n+k(j));
!   !   coefs = reshape(coefs, [prod(sizec(1:j)),sizec(j+1),prod(sizec(j+2:end))]);
!   !   sizec(j+1) = n; coefs = reshape(coefs(:,index,:),sizec)
!   !end if
!!end do
!end function 




![x,y,sizeval,w,origint,p,tolred] = 
subroutine chckxywp(x,y,minn,w,p,tolred)
        implicit none
        double precision,dimension(:),allocatable,target :: x,y
        double precision,dimension(:),allocatable,target,optional :: w
        double precision,intent(in) :: p
!!CHCKXYWP Check and adjust input for *AP*1 commands.
!
!
!! Use only sorted x and prefer finite numbers !
!
!
!
double precision,dimension(:),allocatable :: dx,origint,tmp
integer,dimension(:),allocatable :: ind_nanx,ind_nany,ind_mults,mults,repeats,ignorep,pp
logical,dimension(:),allocatable :: mask_nanx,mask_nany,mask
integer :: n,minn,nstart,n2start,nargin,yn,k,i,nignp,j
logical :: roughnessw,nonemptyw,tosort
double precision :: tolred,temp
n=size(x)
!! deal with NaN's and Inf's among the sites:
!mask_nanx = .not.ieee_is_finite(xi);
!ind_nanx=pack([1:size(xi)],mask_nanx)
!if (any(mask_nanx)) then
!   x=xi(.not. ind_nanx)
!   print *,'SPLINES:CHCKXYWP:NaNs','All data points with NaN or Inf as their site will be ignored.'
!   else
!           x=xi
!end if
!
!n = size(x);
!
!nargin=2+count([present(nmin),present(wi),present(pi)])
!
!if (nargin>2 .and. nmin>0) then 
!        minn = nmin; 
!else 
!        minn = 2;
!end if
!
!if (n<minn) then
!        print *,'SPLINES:CHCKXYWP:toofewpoints','There should be at least',minn,'data sites.'
!end if
!
!! re-sort, if needed, to ensure nondecreasing site sequence:
!!tosort = false;
!!if any(diff(x)<0) then 
!!        tosort = .true.; 
!!        [x,ind] = sort(x); 
!!end if
!
!nstart = n+count(mask_nanx);
!
!! if Y is ND, reshape it to a matrix by combining all dimensions but the last:
!!sizeval = size(y);
!!yn = sizeval(end); sizeval(end) = []; yd = prod(sizeval);
!!if length(sizeval)>1
!!   y = reshape(y,yd,yn);
!!else
!!   ! if Y happens to be a column matrix, of the same length as the original X,
!!   ! then change Y to a row matrix
!!   if yn==1&&yd==nstart
!!      yn = yd; y = reshape(y,1,yn); yd = 1; sizeval = yd;
!!   end
!!end
!!y = y; x = reshape(x,n,1);
!yn=size(yi)
!y=yi
!! make sure that sites, values and weights match in number:
!
!!if (nargin>2 .and. (.not.present(nmin))) then! in this case we accept two more data values than
!!                   ! sites, stripping off the first and last, and returning
!!                   ! them separately, in W, for use in CSAPE1.
!!                   if (yn .eq. nstart+2) then
!!                           w = y([1,yn])
!!                           !y([1,end]) = [];
!!                           if (.not. all(ieee_is_finite(w))) then
!!                                print *,'SPLINES:CHCKXYWP:InfY'
!!                           end if
!!                   else if (yn.eq. nstart) then
!!                          !!!!!!!!!!!!!!!!! !w = [];
!                           allocate(w(0))
!!                  else 
!!                        print *,'SPLINES:CHCKXYWP:XdontmatchY','The number of sites', nstart,'does not match the number of values', yn
!!                end if
!!else
!!   if (yn .ne. nstart) then
!!      print *,'SPLINES:CHCKXYWP:XdontmatchY','The number of sites', nstart,'does not match the number of values', yn
!!   end if
!!end if
!
!nonemptyw = (nargin>3).and. ( (size(w)>0))
!print *,'nonemptyw',nonemptyw
!if (nonemptyw .eq. .true.) then
!   if (size(w) .ne. nstart) then
!      print *,'SPLINES:CHCKXYWP:weightsdontmatchX','The number of weights',size(w),'does not match the number of sites', nstart
!   end if
!end if
!
!roughnessw = present(pi) .and. size(p)>1
!if (roughnessw .eq. .true.) then
!   !if (tosort) then
!   !   print *,'SPLINES:CHCKXYWP:cantreorderrough'
!   !   p = p(1);
!   !else
!      if (size(p) .ne. nstart) then
!         print *,'SPLINES:CHCKXYWP:rweightsdontmatchX','The number of roughness weights is incompatible with the number of sites', nstart
!        end if
!   !end if
!end if
!
!!!! remove values and error weights corresponding to nonfinite sites:
!if (any(mask_nanx)) then
!        !y(nanx,:) = []; 
!        y=yi(.not. ind_nanx)
!        if (nonemptyw) w=wi(.not. ind_nanx) 
!   if (roughnessw)  then! as a first approximation, simply ignore the
!                  ! specified weight to the left of any ignored point.
!      !p(max(nanx,2)) = []; !!!!!!!!! a verifie
!         print *,'WARNING roughnessw not implemented'
!   end if
!end if
!!if (tosort) then
!!        y = y(ind,:); 
!!        if (nonemptyw) w = w(ind)
!!end if
!
!! deal with nonfinites among the values:
!mask_nany = .not. ieee_is_finite(y);
!ind_nany = pack([1:yn],mask_nany) 
!if (any(mask_nany)) then
!   y=y(.not. ind_nany)  
!   x=x(.not. ind_nany)  
!   if (nonemptyw) W= w(.not. ind_nany) 
!   print *,'SPLINES:CHCKXYWP:NaNs','All data points with NaNs or Infs in their value will be ignored.'
!   n = size(x);
!   if (n<minn) then
!      print *,'SPLINES:CHCKXYWP:toofewX','There should be at least',minn,' data sites.'
!      end if
!   if (roughnessw)  then! as a first approximation, simply ignore the
!   !               ! specified weight to the left of any ignored point.
!   !   p(max(nany,2)) = [];
!         print *,'WARNING roughnessw not implemented'
!   end if
!end if
!
nonemptyw=.false.
if (size(w)==0) nonemptyw=.true.
allocate(dx(n-1))
dx = diff(x)
if (nonemptyw) then !  use the trapezoidal rule weights:
   deallocate(w)
   allocate(w(n))
   w = 1d0
   nonemptyw = .not. nonemptyw
   !print *,'nonemptyw',nonemptyw,'w',w
end if

tolred = 0d0
if (.not. all(dx.ne.0d0)) then ! conflate repeat sites, averaging the corresponding values
                 ! and summing the corresponding weights
   allocate(mults(n))
   mults = knt2mlt(x)
   !print *,'mults',mults
   allocate(mask(n))
   mask=diff_i([mults,0])<0
   allocate(ind_mults(count(mask)))
   ind_mults=pack([(i,i=1,size(mults))],mask)
   do k=1,size(ind_mults)
           j= ind_mults(k)
      !if (nonemptyw) then
         temp = sum(w(j-mults(j):j))
         tolred = tolred + dot_product(w(j-mults(j):j),y(j-mults(j):j)**2) 
         y(j-mults(j)) = dot_product(w(j-mults(j):j),y(j-mults(j):j))/temp
         w(j-mults(j)) = temp
         tolred = tolred - temp*(y(j-mults(j))**2);
      !else
      !   y(j-mults(j)) = mean(y(j-mults(j):j)); juste une moyenne
      !end if
   end do
   deallocate(mask)
   allocate(mask(n))
   mask=mults.eq.0
   allocate(repeats(count(mask)))
   repeats = pack([(i,i=1,size(mults))],mults.eq.0);
   n=size(repeats)
   allocate(tmp(n))
   tmp=x(repeats) 
   call move_alloc(tmp,x)
   allocate(tmp(n))
   tmp=y(repeats) 
   call move_alloc(tmp,y)
   allocate(tmp(n))
   tmp=w(repeats) 
   call move_alloc(tmp,w)
   !if (roughnessw)  then ! as a first approximation, simply ignore the
                  ! specified weight to the left of any ignored point.
   !   p(max(repeats,2)) = [];
   !      print *,'WARNING roughnessw not implemented'
   !end if
   n = size(x);
   if (n<minn) print *,'There should be at least',minn,' data sites.'
   deallocate(mask)
end if

deallocate(dx)
if(autoweight .eqv. .true.) then
allocate(dx(n-1))
dx = diff(x)
if (all(dx.ne.0d0)) then 
        w=w*(([dx,0d0]+[0d0,dx])*0.5d0)
end if
end if
! remove all points corresponding to relatively small weights (since a
! (near-)zero weight in effect asks for the corresponding datum to be dis-
! regarded while, at the same time, leading to bad condition and even
! division by zero).

allocate(origint(0))    ! this will be set to x([1 end]).' in case the weight for an end
             ! data point is near zero, hence the approximation is computed
             ! without that endpoint.

allocate(mask(n))
mask= w <= (1e-13)*maxval(abs(w)) 
allocate(ignorep(count(mask)))
ignorep = pack([(i,i=1,size(w))],mask )
nignp=size(ignorep)
if (nignp>0) then
        if (ignorep(1)==1 .or. ignorep(nignp)==n ) then
                allocate(origint(2))
                origint = x([1,nignp])
                print *,'!!!!!!!origint Not supported yet stop here!!!!!!!'
        end if
        !x(ignorep) = [];
        allocate(pp(count(.not.mask)))
        pp=pack([(i,i=1,size(w))],.not.mask )
        print *,'pp',lbound(pp),pp
        n = size(pp);
        allocate(tmp(n))
        tmp=x(pp)
        call move_alloc(tmp,x)
        !y(ignorep,:) = [];
        allocate(tmp(n))
        tmp=y(pp)
        call move_alloc(tmp,y)
        !w(ignorep) = []; 
        allocate(tmp(n))
        tmp=w(pp)
        call move_alloc(tmp,w)
        if (n<minn) then
                print *,'There should be at least !g data points with positive weights.',minn
        end if
end if

!
end subroutine chckxywp


function mean(x)
        double precision,dimension(:),intent(in) :: x
        double precision :: mean
        integer :: n
        n=size(x)

        mean=sum(x(:))/n

end function mean

function  knt2mlt(t) result(m)
double precision,dimension(:),intent(in) :: t
integer ,dimension(:),allocatable :: m

!KNT2MLT Knot multiplicities.
!
!   KNT2MLT(T) returns the vector M of knot multiplicities.

double precision,dimension(:),allocatable :: difft
integer,dimension(:),allocatable :: zz,pt,z
integer,dimension(:),allocatable :: index
logical,dimension(:),allocatable :: mask
integer :: n,i
![r,c] = size(t);
n=size(t)
allocate(difft(n-1))
difft = diff(t) 
!if (any(difft<0))
        !t = sort(t) 
!        difft = diff(t)
!end
allocate(index(n),m(n),mask(n-1))
index = 0
index(2:n)=abs(transfer(difft==0d0,index(2:n)))
m = cumsum(index)
mask=diff_i(index)<0
allocate(zz(count(mask)))
zz = pack([(i,i=1,n-1)],mask)

if (size(zz).eq.0) return
allocate(z(n),pt(size(zz)))
z = 0
pt = m(zz)
pt(2:) = diff_i(pt)
z(zz+1) = pt
m = m - cumsum(z)
deallocate(z,index,zz,pt)
end function knt2mlt

function cumsum(arr,seed)
        integer, dimension(:), intent(in) :: arr
        integer, optional, intent(in) :: seed
        integer, dimension(size(arr))  :: cumsum
        integer :: n,j
        
        double precision :: sd
        n=size(arr)
        if (n == 0) return
        sd=0.0
        if (present(seed)) sd=seed

        cumsum(1)=arr(1)+sd

        do j=2,n
                cumsum(j)=cumsum(j-1)+arr(j)
        end do


end function cumsum


function diff_i(x) 
        integer,dimension(:),intent(in) :: x
        integer,dimension(:),allocatable :: diff_i
        integer :: n
        n=size(x)
        allocate(diff_i(n-1))
        diff_i=x(2:n)-x(1:n-1)

end function diff_i

function diff(x) 
        double precision,dimension(:),intent(in) :: x
        double precision,dimension(:),allocatable :: diff
        integer :: n
        n=size(x)
        allocate(diff(n-1))
        diff=x(2:n)-x(1:n-1)

end function diff

end module spline
