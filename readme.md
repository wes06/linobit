# Linobit
## A recreational core rope ROM device


### Abstract:

This paper explores the decision making process of making Linobit, a scaled down and portable version of "Linobyte - A core rope ROM based experiment". From the exploration of the technical aspects of the original core ROM devices from the 60s to the functionality and decisiosn regarding modern circuitry.





### Introduction to Core Rope ROMs

Core Rope Read Only Memories were devices used by computers in the 60s and 70s to store permanent information. They were a fast and robust alternative to other memories of the time (punched cards and ) and were used in historically famous computers such as the AGC - Apollo Guidance Computer. In the case of space bound applications, the ruggedness of Core Rope ROMs against cosmic radiation made them a reliable choice. Despite these positive aspects though, they were expensive and extremely labourious to build - the information was encoded through hand winding of an enameled copper wire into ferrite toroids. (It was referred to as "LOL" memory by Raytheon Engineers: Little Old Lady memory).

### Introduction to Linobyte (Linobits parent project)

"As computational devices evolve, more tools and interfaces are built between the user and the machine. This allows us to complete increasingly complex tasks without having to focus so much on understanding the nuances of the machine. While this movement is certainly overall positive, one of the drawbacks is that people no longer learn the fundamental processes and concepts which allow the tool to work. Added to that, by neglecting history, we forget the alternatives of the technologies that we use today - forgotten alternatives that maybe once were the status quo. Understanding of those alternatives would give us a broader view of the pros and cons of what we have today, how they superseded their ancestors and what are their pitfalls - important knowledge for those who design possible futures.

Its with these preocupations in mind that Linobyte came into existence. It conciliates the explanation of how bits, bytes and chars work, with a hands on experience of creating Core Rope ROMs: read-only memories that were written by weaving an enameled copper wire through ferrite cores."

https://github.com/wes06/linobyte


### Introduction to Linobits

Linobyte features 8 bytes with 8 bits each, which can represent an 8 character word, and therefore is quite relatable to the general public. However, this makes it quite complex and expensive to reproduce, and the multiple functional blocks make it hard to understand in a quick glance. Linobit was born to address these issues, striping down unnecessary and redundant power circuits, replacing modern ICs with with discrete logic, reducing the amount of coils, and using no microcontrollers.

### Coil drive and read system

In Linobyte, a brute force approach was used. There is a permanent oscillator on the main board, which drives individual character blocks as soon as they are enabled. This oscillator in turn drives a local gate driver that drives the wire that goes through the coils with a series capacitor. Each coil therefore acts as a transformer with a single turn on the primary and a rectifier block on the secondary side. The rectifier block charges a capacitor which in turn feeds an analog comparator, that has a V ref that comes from the main board. This approach was taken in Linobyte to avoid the necessity of latches and resets, allowing each bit to be charged independently of microcontroller timing, reducing the need for the whole logic process (set, reset, multiplex chars) to be defined during board design.

In Linobit, since the amount of coils is greatly reduced (from 64 to 4) it is easier to experiment with latches and syncronous execution processes. Therefore, the coil is not driven by a permanent oscillator but instead by a pulse that is triggered by the press of the button. The pulse then passes through the transformers that are "set" - as in, that have a wire passing through them - and sets the latches on the secondary side. Instead of a rectifier charging a capacitor - needing multiple pulses to reach the trigger level - the rectifier simply triggers the SR Latch.

### SR Latch

SR stands for SET and RESET, that are the two inputs on this circuit. It is composed of two XOR gates, which in the case of Linobit, I have decided to build discretely (instead of using integrated circuits). The SET input is connected to the output of the rectifier, and the RESET is connected to a button controlled by the user.

The output of this circuit is connected to a 4 to 10 BCD to 7 segment converter IC.

### 4 to 10 BCD to 7 Segment display


