; Domaine : le robot mobile SAM (base mobile, pince, bras).
; Philippe Morignot - 19/01/2010.
; Revu le 20/01/2010. Revu le 21/01/2010. Revu le 27/01/2010. Revu le 28/01/2010. Revu le 29/01/2010. Revu le 01/02/2010.

(define (domain sam)
   ;;(:requirements :strips)
   (:types scene objet point3D point2D rotation3D angle)
   (:predicates
  	(approche-x-y-z ?os ?pt3D)		; Les objets dans la scène
        (approche-rx-ry-rz ?os ?rot)
	(position ?o ?pt2D)

	(chemin ?pt2D1 ?pt2D2)			; La carte topologique

        (bras-pose ?pt3D ?rot)			; Le bras
	(bras-securite  ?pt3D ?rot)
	(bras-transport ?pt3D ?rot)
	(bras-depose ?pt3D ?rot)

	(initialisation-bras) (not-initialisation-bras)
  	(obstacles-evites) (not-obstacles-evites)

	(pince-ouverte) (not-pince-ouverte)	; La pince
	(pince-tient ?o)

        (base-mobile ?pt2D ?a)			; La base mobile.

	(evt-objet-selectionne ?o)		; Les événements attendus, comme résultats d'actions.
 	(evt-barriere-optique-franchie)

	(usr-zone-choisie ?s ?o)		; Pré définition de l'objet choisi par l'utilisateur (hum, hum !)
   )

; Déplace la base mobile de (?pt1 ?a) à (?pt2 ?a) parce qu'il y a un chemin de ?pt1 à ?pt2
; Cette action dés-initialise le bras

(:action deplace-base-mobile
   :parameters (?pt1 - point2D ?pt2 - point2D ?pt - point3D ?r - rotation3D ?a - angle)
   :precondition (and (base-mobile ?pt1 ?a) (chemin ?pt1 ?pt2)
		      (bras-transport ?pt ?r) (bras-pose ?pt ?r) (not-pince-ouverte))
   :effect (and (base-mobile ?pt2 ?a) (not (base-mobile ?pt1 ?a))
                (not (initialisation-bras)) (not-initialisation-bras)))

; Les paramètres ?pt ?a ?pt2 et ?rot2 ne sont là que pour différencier le nom de l'action.

(:action initialise-bras
   :parameters (?pt - point2D ?a - angle ?pt2 - point3D ?r - rotation3D)
   :precondition (and (not-initialisation-bras) (base-mobile ?pt ?a) (bras-pose ?pt2 ?r))
   :effect (and (initialisation-bras) (not (not-initialisation-bras))))

; Prend la spécification générale de dépose d'un objet (?pt2 ?rot2) et aligne la position du bras dessus.
; Puisque cette action bouge le bras, ce bras a besoin d'avoir été initialisé.

(:action positionne-bras-pour-depose
   :parameters (?pt - point3D ?rot - rotation3D ?pt2 - point3D ?rot2 - rotation3D)
   :precondition (and (bras-pose ?pt ?rot) (bras-depose ?pt2 ?rot2) (initialisation-bras))
   :effect (and (not (bras-pose ?pt ?rot)) (bras-pose ?pt2 ?rot2)))

(:action ouvre-pince-tenant-objet
   :parameters (?o - objet ?pt - point2D ?a - angle ?pt2 - point3D ?rot2 - rotation3D)
   :precondition (and (pince-tient ?o) (base-mobile ?pt ?a)
		      (bras-depose ?pt2 ?rot2) (bras-pose ?pt2 ?rot2))
   :effect (and (not (pince-tient ?o)) (not (not-pince-ouverte)) (pince-ouverte) (position ?o ?pt)))

; Action qui fait passer la pince de fermée à ouverte.

(:action ouvre-pince
   :parameters ()
   :precondition      (not-pince-ouverte)
   :effect (and  (not (not-pince-ouverte)) (pince-ouverte)))

(:action positionne-bras-devant-scene
   :parameters (?s - scene ?pt2 - point3D ?rot2 - rotation3D ?pt - point3D ?rot - rotation3D)
   :precondition (and (approche-x-y-z ?s ?pt) (approche-rx-ry-rz ?s ?rot)
		      (bras-pose ?pt2 ?rot2) (not-pince-ouverte) (initialisation-bras))
   :effect (and (bras-pose ?pt ?rot) (not (bras-pose ?pt2 ?rot2))))

(:action positionne-bras-devant-objet
   :parameters (?o - objet ?pt2 - point3D ?rot2 - rotation3D ?pt - point3D ?rot - rotation3D)
   :precondition (and (approche-x-y-z ?o ?pt) (approche-rx-ry-rz ?o ?rot)
		      (bras-pose ?pt2 ?rot2) (not-pince-ouverte) (initialisation-bras))
   :effect (and (bras-pose ?pt ?rot) (not (bras-pose ?pt2 ?rot2))))

(:action positionne-bras-pour-securite
    :parameters (?pt1 - point3D ?rot1 - rotation3D ?pt2 - point3D ?rot2 - rotation3D)
    :precondition (and (bras-pose ?pt1 ?rot1)  (bras-securite ?pt2 ?rot2))
    :effect (and  (not (bras-pose ?pt1 ?rot1)) (bras-pose ?pt2 ?rot2)))

(:action positionne-bras-pour-transport
    :parameters (?pt1 - point3D ?rot1 - rotation3D ?pt2 - point3D ?rot2 - rotation3D)
    :precondition (and (bras-pose ?pt1 ?rot1)  (bras-transport ?pt2 ?rot2))
    :effect (and  (not (bras-pose ?pt1 ?rot1)) (bras-pose ?pt2 ?rot2)))

(:action ferme-pince
   :parameters ()
   :precondition      (pince-ouverte)
   :effect  (and (not (pince-ouverte)) (not-pince-ouverte)))

; On sait que la barriere optique va être franchie, en supposant que la stéréoscopie a bien évalué la distance.

(:action evite-obstacle-vers-objet
   :parameters (?o - objet ?pt - point3D ?r - rotation3D ?pto - point2D ?a - angle)
   :precondition (and (bras-pose ?pt ?r)
		      (evt-objet-selectionne ?o)
                      (approche-x-y-z ?o ?pt) (approche-rx-ry-rz ?o ?r)
		      (position ?o ?pto) (base-mobile ?pto ?a)
                      (pince-ouverte) (not-obstacles-evites))
   :effect (and (obstacles-evites) (not (not-obstacles-evites))))

(:action avance-en-aveugle-vers-objet
   :parameters (?o - objet ?pt - point3D ?r - rotation3D ?pto - point2D ?a - angle)
   :precondition (and (bras-pose ?pt ?r)
		      (evt-objet-selectionne ?o)
                      (approche-x-y-z ?o ?pt) (approche-rx-ry-rz ?o ?r)
		      (position ?o ?pto) (base-mobile ?pto ?a)
                      (pince-ouverte) (obstacles-evites))
   :effect (evt-barriere-optique-franchie))

; Si la barriere optique est franchie, alors fermer la pince.

(:action ferme-pince-sur-objet
   :parameters (?o - objet ?pt - point3D ?r - rotation3D ?pto - point2D ?a - angle)
   :precondition (and (bras-pose ?pt ?r)
		      (evt-objet-selectionne ?o)
                      (approche-x-y-z ?o ?pt) (approche-rx-ry-rz ?o ?r)
		      (position ?o ?pto) (base-mobile ?pto ?a)
                      (pince-ouverte)
                      (evt-barriere-optique-franchie))
   :effect (and (not-pince-ouverte) (not (pince-ouverte)) (pince-tient ?o)))

; Action utilisateur

(:action choisit-objet-dans-scene
   :parameters (?s - scene ?o - objet ?pt - point3D ?r - rotation3D)
   :precondition (and (approche-x-y-z ?s ?pt) (approche-rx-ry-rz ?s ?r)
                      (bras-pose ?pt ?r)
		      (usr-zone-choisie ?s ?o))				; Prédicat (sensitif) pour choix utilisateur
   :effect (evt-objet-selectionne ?o))

)