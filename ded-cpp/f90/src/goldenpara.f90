module goldenpara
        

contains
function brent(ax,cx,func,tol,xmin)
implicit none
double precision, intent(in) :: ax,cx,tol
double precision, intent(out) :: xmin
double precision :: brent,bx
interface
        function func(x)
                double precision, intent(in) :: x
                double precision :: func
        end function func
end interface
integer, parameter :: itmax=100
double precision, parameter :: cgold=(3d0-sqrt(5d0))/2d0,zeps=1d-3*epsilon(ax)
!given a function func, and given a bracketing triplet of abscissas ax, bx, cx (such that bx
!is between ax and cx, and func(bx) is less than both func(ax) and func(cx)), this
!routine isolates the minimum to a fractional precision of about tol using brent’s method.
!the abscissa of the minimum is returned as xmin, and the minimum function value is
!returned as brent, the returned function value.
!parameters: maximum allowed number of iterations; golden ratio; and a small number that
!protects against trying to achieve fractional accuracy for a minimum that happens to be
!exactly zero.
integer :: iter
double precision :: a,b,d,e,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm
a=min(ax,cx)
!a and b must be in ascending order, though
b=max(ax,cx)
!the input abscissas need not be.
!v=bx choix du point de depart , en cas de tolerance par default par exemple
v=a+cgold*(b-a) ! R version du starting point
!Initializations...
w=v
x=v
e=0.0
!This will be the distance moved on the step
fx=func(x)
!before last.
fv=fx
fw=fx
do iter=1,ITMAX
        !Main program loop.
        xm=0.5d0*(a+b)
        tol1=tol*abs(x)+ZEPS
        tol2=2.0d0*tol1
        if (abs(x-xm) <= (tol2-0.5d0*(b-a))) then
                !Test for done here.
                xmin=x
                !Arrive here ready to exit with best values.
                brent=fx
                return
        end if
        if (abs(e) > tol1) then
                !Construct a trial parabolic fit.
                r=(x-w)*(fx-fv)
                q=(x-v)*(fx-fw)
                p=(x-v)*q-(x-w)*r
                q=2.0d0*(q-r)
                if (q > 0d0) p=-p
                q=abs(q)
                etemp=e
                e=d
                if (abs(p) >= abs(0.5d0*q*etemp) .or.  p <= q*(a-x) .or. p >= q*(b-x)) then
                        !The above conditions determine the acceptability of the parabolic fit. Here it is
                        !not o.k., so we take the golden section step into the larger of the two segments.
                        e=merge(a-x,b-x, x >= xm )
                        d=CGOLD*e
                else
                        !Take the parabolic step.
                        d=p/q
                        u=x+d
                        if (u-a < tol2 .or. b-u < tol2) d=sign(tol1,xm-x)
                end if

        else
                !Take the golden section step into the larger
                e=merge(a-x,b-x, x >= xm )
                !of the two segments.
                d=CGOLD*e
        end if
        u=merge(x+d,x+sign(tol1,d), abs(d) >= tol1 )
        !Arrive here with d computed either from parabolic fit, or else from golden section.
        fu=func(u)
 
        !This is the one function evaluation per iteration.
        if (fu <= fx) then
                !Now we have to decide what to do with our
                if (u >= x) then
                        !function evaluation. Housekeeping follows:
                        a=x
                else
                        b=x
                end if
                call shft(v,w,x,u)
                call shft(fv,fw,fx,fu)
        else
                if (u < x) then
                        a=u
                else
                        b=u
                end if
                if (fu <= fw .or. w == x) then
                        v=w
                        fv=fw
                        w=u
                        fw=fu
                else if (fu <= fv .or. v == x .or. v == w) then
                        v=u
                        fv=fu
                end if
        end if
end do  !Done with housekeeping. Back for another iteration.
print *,'brent: exceed maximum iterations'
brent=fx
xmin=x
contains
subroutine shft(a,b,c,d)
double precision, intent(out) :: a
double precision, intent(inout) :: b,c
double precision, intent(in) :: d
a=b
b=c
c=d
end subroutine shft
end function brent
end module goldenpara

