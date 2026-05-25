#pragma once
#include "../data.h"

//Calcola se c'è un'intersezione, ed eventualmente da che lato
template<typename A, typename B>
inline bool CheckAABB(const A& a, const B& b, bool& hitSideX, bool& hitSideY) 
{
    //Controlla se A e B sicuramente non si toccano (sono distanti)
	if (a.Right() <= b.Left()) return false;  //A è più a destra di B
	if (a.Left() >= b.Right()) return false;  //A è più a sinistra di B
	if (a.Bottom() <= b.Top()) return false;  //A è più in basso di B
	if (a.Top() >= b.Bottom()) return false;  //A è più in alto di B

    //Nessuna delle quattro condizioni si è verificata -> A e B si toccano

    //Calcola i bordi del rettangolo di sovrapposizione
    float rBord, lBord, bBord, tBord;
    if (a.Right() < b.Right()) { rBord = a.Right(); } else { rBord = b.Right(); }		//bordo dx di sovrapposizione
	if (a.Left() > b.Left()) { lBord = a.Left(); } else { lBord = b.Left(); }			//bordo sx di sovrapposizione
	if (a.Bottom() < b.Bottom()) { bBord = a.Bottom(); } else { bBord = b.Bottom(); }   //bordo sotto di sovrapposizione
	if (a.Top() > b.Top()) { tBord = a.Top(); } else { tBord = b.Top(); }               //bordo sopra di sovrapposizione
	
	//La distanza minore indica il lato di impatto (es. se overlapX < overlapY -> impatto laterale)
	float overlapX = rBord - lBord;	//Sovrapposizione orizzontale
	float overlapY = bBord - tBord; //Sovrapposizione verticale

	if (overlapX < overlapY) { hitSideX = true; } else { hitSideX = false; }
    hitSideY = !hitSideX;

	/*
		Idea:	la palla ha sovrapposizione minore in base al lato in cui arriva

				┌──────┐
			┌───┼──┐   │   overlapX minore -> la palla viene da sinistra/destra -> hitSideX = true
			│   └──┼───┘       
			└──────┘

			┌──────┐
			│  ┌───┼───┐   overlapY minore -> la palla viene dall'alto/basso -> hitSideY = true
			│  └───┼───┘   
			└──────┘

		Nota: falla se arriva esattamente in diagonale, casistica rara

		Link dove spiegano un po' la cosa:
		-https://www.jeffreythompson.org/collision-detection/rect-rect.php
		-https://gamedev.stackexchange.com/questions/24078/which-side-was-hit
	*/

    return true;
}


//Conversione tra pixel e NDC ("""spazio della GPU""")
inline float ToNDC_X(float px) { return (px / SCREEN_WIDTH) * 2.0f - 1.0f; }
inline float ToNDC_Y(float py) { return 1.0f - (py / SCREEN_HEIGHT) * 2.0f; }