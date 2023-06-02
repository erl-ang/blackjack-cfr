TODO: change Makefile to cpp20



Couple things
    ---- There are some theoretic very weird branches that will almost never happen, I think we can 
            account for them without a ton of added overhead (and adds some complexity that might be good)
            but since our game will almost never get there, it won't rly converge to useful values.

            Ex: Draw (A) --> Draw (A) --> Draw (A) --> Draw (A) --> Draw(A)
            
            The likelihood of this is very low but we would have a tree depth of 20 in this situation.

    --- Initialize game with dynamic sum (might need bigger tree)