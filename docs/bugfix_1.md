# BUG
- so i have realized that i had overlooked the most textbook definition of the thing i was using all this time, `pointers` as the name says it points at memory addresses
- so that will mean that it is not an immutable value, infact it isnt a value at all it just points at things
- so for saving the words as tokens i do `TOKENS[i].tok_word = &buffer[p]` im storing the address of `buffer[p]` in `tok_word`
- so when i save a char and then "put it back" `tok_word` doesnt see the null char anymore so it takes the whole thing

# FIX
- i just copy it into a variable and then store that into `tok_word` + `malloc` and `free` 

# INSIGHT
- now i have to go thru all the circus i had done earlier and check which of it is needed, and which of it is redundant
- THIS WAS SUCH A STUPID OVERSIGHT ITS INSANE 

# FINALLY
- even with all the clutter it works.
- make it exist and then make it pretty as they say