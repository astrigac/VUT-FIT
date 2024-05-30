% Zadani c. 33:
% Napiste program resici ukol dany predikatem u33(LIN,LOUT), kde LIN je vstupni 
% celociselny seznam a LOUT je vystupni seznam obsahujici vsechny prvky seznamu 
% LIN, ve kterem jsou vsechna suda cisla prevedena na licha odectenim jednicky. 

% Testovaci predikaty:                                 	% LOUT 
u33_1:- u33([5,-3,4,8,3,1,0,-2], LOUT),write(LOUT).	% [5,-3,3,7,3,1,-1,-3]
u33_2:- u33([10,3,1,7],LOUT),write(LOUT).		% [9,3,1,7]
u33_3:- u33([],LOUT),write(LOUT).			% []
u33_r:- write('Zadej LIN: '),read(LIN),
	u33(LIN,LOUT),write(LOUT).

% Reseni:
u33([], []). 
u33([H|T], [H1|TT]) :- H mod 2 =:= 0, !, H1 is H - 1, u33(T, TT).
u33([H|T], [H|TT]) :- u33(T, TT).
