(define (problem sam03)
  (:domain sam)
  (:objects table - scene
	    canette-coca - objet
            ptr1 ptr2 ptr3 ptrt ptrd ptrs - point3D
	    rotr1 rotr2 rotr3 rotrt rotrd rotrs - rotation3D
	    pt1 pt2 pt3 pt4 pt5 pt6 pt7 pt8 pt9 pt10 pt11 pt12 pt13 pt14 pt15 pt16 pt17 pt18 pt19 pt20 pt21 pt22 - point2D
 	    abm - angle
	)
  (:init
	(approche-x-y-z table ptr1)			; Etat initial de la scène
	(approche-rx-ry-rz table rotr1)

	(approche-x-y-z canette-coca ptr2)
        (approche-rx-ry-rz canette-coca rotr2)
	(position canette-coca pt2)

	(chemin pt1 pt2) (chemin pt2 pt1)		; Carte topologique, mais non linéaire.
	(chemin pt2 pt3) (chemin pt3 pt2)
	(chemin pt3 pt4) (chemin pt4 pt3)
	(chemin pt4 pt5) (chemin pt5 pt4)
	(chemin pt5 pt6) (chemin pt6 pt5)
	(chemin pt6 pt7) (chemin pt7 pt6)
	(chemin pt7 pt8) (chemin pt8 pt7)
	(chemin pt8 pt9) (chemin pt9 pt8)
	(chemin pt9 pt10) (chemin pt10 pt9)
	(chemin pt10 pt11) (chemin pt11 pt10)
	(chemin pt11 pt12) (chemin pt12 pt11)
	(chemin pt12 pt13) (chemin pt13 pt12)
	(chemin pt13 pt14) (chemin pt14 pt13)
	(chemin pt14 pt15) (chemin pt15 pt14)
	(chemin pt15 pt16) (chemin pt16 pt15)

	(chemin pt9 pt17) (chemin pt17 pt9)
	(chemin pt17 pt18) (chemin pt18 pt17)
	(chemin pt18 pt19) (chemin pt19 pt18)
	(chemin pt19 pt20) (chemin pt20 pt19)
	(chemin pt20 pt21) (chemin pt21 pt20)
	(chemin pt21 pt22) (chemin pt22 pt21)

	(base-mobile pt1 abm)				; Etat initial de la base mobile

	(bras-pose ptr3 rotr3)				; Etat initial du bras
	(bras-securite ptrs rotrs)       		; Constante pour la position de sécurité
	(bras-transport ptrt rotrt)			; Constante pour le transport
	(bras-depose ptrd rotrd) 			; Constante pour la dépose

	(not-pince-ouverte)				; Etat initial de la pince

	(not-initialisation-bras)                  	; Etat initial du bras
	(not-obstacles-evites)

	(usr-zone-choisie table canette-coca)		; Deviner ce que l'utilisateur va choisir ... hum hum !
	)
  (:goal
        (and
	   (position canette-coca pt15)
;	   (position canette-coca pt22)
	)))
