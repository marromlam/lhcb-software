C
C
C
C
C
        FUNCTION SGMIN(N)
        IMPLICIT REAL*8(A-H,O-Z)
        GA=0.
        IF(N.LE.2) GO TO 20
        DO 10 I=1,N-1
        Z=I
        GA=GA+LOG(Z)
10      CONTINUE
20      SGMIN=GA
        RETURN
        END
