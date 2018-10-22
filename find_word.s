 .data
    
    input:        .asciiz  "\nEnter input: " 
    
    out:         .asciiz"\noutput:\n"
    newline:        .asciiz  "\n"
    null:           .asciiz  "\0"
    sentence:       .space 1001
    word:           .space 51
     
     .globl main
     .text  
     
     li $t4, 0          #   input index  
     li $t5, 0           #   is end sentence
     la $s7,word          #   point word
      #   point first char
main: li $t6 ,0         #is word found
      jal read_input
      # print_string("\noutput:\n");
      li $v0, 4           
      la $a0,out
      syscall
      # do {word_found = process_input(input_sentence, word);
           #if ( word_found == true ) {
           # output(word);}} while ( word_found == true );
      j process_input 
   back:  beqz $t6, end 
      jal output
      j process_input
      # end main
   end: li $v0, 10
         syscall       
            
output: #void output(const char* out) {
        # print_string(out);
        # print_string("\n");}
        li $v0, 4           
        la $a0,word
        syscall

        li $v0, 4           
        la $a0,newline
        syscall
        
        jr $ra                           
                       
                                  
read_input:  #void read_input(const char* inp) {
    #print_string("\nEnter input: ");
    #read_string(input_sentence, MAX_SENTENCE_LENGTH+1);}    
          li $v0, 4           
          la $a0,input
          syscall
          
          #void read_input(const char* inp) {
          #print_string("\nEnter input: ");
    #read_string(input_sentence, MAX_SENTENCE_LENGTH+1);}
          li $v0, 8
          la $a0,sentence
          li $a1, 1001       #max 1001 chars
          move $s6,$a0     #   point first char
          syscall
            jr $ra   
            
                   
process_input: 
               li $t1,0    #char cur_char = '\0';
               li $t2,0    #int is_valid_ch = false;
               li $t3 ,-1   #int char_index = -1;
     while: beq $t5, 1,endwhile  #while( end_of_sentence == false ) else return false     
           #cur_char = inp[input_index];
           addu $t0, $t4, $s6      #get address of current char
           lb $t1, ($t0)           #save current char to t1
           #input_index++;
           addiu $t4,$t4,1   
           
               #is_valid_ch = is_valid_character(cur_char);
               
           jal ischar
                #if ( is_valid_ch ) (to savechar)
           bnez $t2 , savechar
                #if is not valid char go below
               
           beqz $t1, endsentence   #whether cur_char is null, endsentence
                 #whether cur_char is newline,endsentence
           la $a0,newline      #a0 point to newline
           lb $t9, ($a0)       #set t9 to be newline
           beq $t1,$t9,endsentence
                 #if ( cur_char == '\n' || cur_char == '\0' ) {
                  
                       bltz $t3,skipchar    #if it's first char, skip this char else go below
                         #if ( is_hyphen(cur_char) == true && is_valid_character(inp[input_index]) )
                        jal ishyp
                        beqz $t2,endword  #if current char is not ’-‘, endowed
                       lb $t1, 1($t0)  #load next char
                        jal ischar     
                        beqz $t2,endword    #if next char is not valid char,endword
                        #if it's valid char, save char
                        addu $t0, $t4, $s6 #t0 point to current char     
                        lb $t1, -1($t0)     #reset curent char
                       j savechar
                        
  endword:    addiu $t3,$t3,1 #char_index++;
              #w[char_index] = '\0';
              addu $a0,$s7,$t3  
              sb $0,($a0)    
              li $t6, 1    #return true 
              j back  
endsentence:         li $t5, 1
                     j while               
  skipchar:sb $0 ,($s7)
            li $t3,-1
              j while      
    savechar:  #w[++char_index] = cur_char; 
              addiu $t3, $t3,1 
              addu $t0,$t3,$s7 
              sb $t1,($t0)
             j while 
             
     endwhile: li  $t6, 0        
               j back
               
ischar: blt $t1,65, notchar
       bgt $t1,122,notchar
       sge $t9, $t1,91
       sle $t8,$t1,96
       beqz $t8, char
       beq $t9,$t8,notchar
       
      char:  li $t2,1
             jr $ra
    notchar: li $t2,0   
      jr $ra
      
ishyp: beq $t1,45,is
       li $t2,0
       jr $ra
       is: li $t2,1
      jr $ra                                                                                        
