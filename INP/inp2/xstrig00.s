
; Autor reseni: Aurel Strigac xstrig00
; Pocet cyklu k serazeni puvodniho retezce:             2519
; Pocet cyklu razeni sestupne serazeneho retezce:       3200
; Pocet cyklu razeni vzestupne serazeneho retezce:      175
; Pocet cyklu razeni retezce s vasim loginem:           627
; Implementovany radici algoritmus: Bubble Sort
; ------------------------------------------------

; DATA SEGMENT
                .data
; login:          .asciiz "vitejte-v-inp-2023"  ; puvodni uvitaci retezec
; login:          .asciiz "vvttpnjiiee3220---"  ; sestupne serazeny retezec
; login:          .asciiz "---0223eeiijnpttvv"  ; vzestupne serazeny retezec
  login:          .asciiz "xstrig00"            ; SEM DOPLNTE VLASTNI LOGIN
                                                ; A POUZE S TIMTO ODEVZDEJTE

params_sys5:    .space  8   ; misto pro ulozeni adresy pocatku
                            ; retezce pro vypis pomoci syscall 5
                            ; (viz nize - "funkce" print_string)

; CODE SEGMENT
                .text

main:

        ; SEM DOPLNTE VASE RESENI

        Loop:
                        lb r15, 1(r10)          ; Load the next byte 
                        lb r14, 0(r10)          ; Load current byte
                        beqz r15, CheckSwap     ; If the next byte is null, go to CheckSwap

                        slt r16, r15, r14       ; Set r16 to 1 if r14 > r15
                        addi r9, r10, 0         ; Create index for easier access
                        addi r10, r10, 1        ; Increment the pointer
                        beqz r16, Loop          ; If r16 is 0, go to Loop

                        addi r13, r0, 1         ; Set the swap flag
                        sb r15, 0(r9)           ; Swap the bytes
                        sb r14, 0(r10)

        NoSwap:
                        j Loop     ; Repeat the loop

        CheckSwap:
                        addi r10, r0, login     ; Reset the pointer to the beginning
                        beqz r13, EndBubbleSort ; If no swaps occurred, go to EndBubbleSort
                        addi r13, r0, 0         ; Clear swap flag
                        j Loop                  ; Otherwise, repeat the loop

        EndBubbleSort:
                        jal print_string        ; Call the print_string subroutine
                        
                        syscall 0               ; halt


print_string:                                   ; adresa retezce se ocekava v r4
                sw      r4, params_sys5(r0)
                daddi   r14, r0, params_sys5    ; adr pro syscall 5 musi do r14
                syscall 5                       ; systemova procedura - vypis retezce na terminal
                jr      r31                     ; return - r31 je urcen na return address
