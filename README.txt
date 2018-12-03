======================================
 EECE 7205 Project 2: Task Scheduling
 Keelin Becker-Wheeler
======================================

This is an implementation of the algorithms discussed in the file "Task Scheduling.pdf":

  Lin, Xue & Wang, Yetang & Xie, Qing & Pedram, Massoud. (2014). Energy and Performance-Aware
  Task Scheduling in a Mobile Cloud Computing Environment. IEEE International Conference on
  Cloud Computing, CLOUD. 192-199. 10.1109/CLOUD.2014.35.


--------------------
"eece7205_proj2_presentation.pdf"
--------------------

 - the powerpoint presentation with sample output (converted to pdf)

 - the presentation references "sample42_out.txt", unfortunately I accidently overwrote these
 raw data files and the data is lost, see "sample/" for another example with raw output data

 - The N=7 sample shows an interesting case where task migration improves time and energy
 over initial scheduling, this is possible in the algorithm since task 0 has an earlier
 finish time on the local core and so is initially scheduled locally. However, it turns out
 that all other tasks may begin earlier by migrating this task 0 to the cloud, resulting in reduction in the total time.
    We also see that the baseline 1 (random assignment) algorithm does better as well. This
 is possible because the MCC algorithm will place task 0 onto the local core initially,
 resulting in task 5 being placed on the local core as well. MCC will then correctly migrate
 task 0 onto the cloud, but will only migrate one task at a time so task 5 is left behind.
 Baseline 1 shows that an optimal scheduling places both these tasks onto the cloud, and so
 we see one drawback with the MCC algorithm: it will only migrate one task at a time and so
 will not always find the most optimal solution. We see in the data (last two slides of
 presentation) that the benefits of the MCC algorithm outweigh this drawback, however.
    The N=7 sample slides show the input data used in the run, so it can be reproduced if
 desired. However, the N=42 sample references an external file that has been lost, so it is
 not reproducable.

 - The N=42 sample is less interesting, but rather shows the general case that MCC does very
 well, especially for higher values of N. The data shows this trend as well, up to N=1000
 (yes, I ran sequentially up to N=1000 using automated scripts running overnight). Note that
 the use of a random algorithm with 10000 samples in baseline 1 becomes indistinguishable
 from using just a single sample as N grows, i.e. for K=3 and N=7 we already have 16384
 possible schedules, so 10000 iterations is increasingly unlikely to give the optimal result
 for any larger value of N. A better analysis would look for the optimal scheduling out of
 all possible schedulings and compare the MCC schedule to this optimal schedule. One such
 method to get an optimal schedule is to adapt baseline 1 into an algorithm that goes over
 all (K+1)^N schedules rather than 10000 random ones. This could become very costly however,
 which may be why the research paper made no such attempt to compare to a true optimal
 schedule. I also made no attempt to perform this extra analysis.
    Note also that the research paper mentions a maximum time, this is the value T_MAX in my
 program, which is calculated as 1.5 times the initial scheduling time. This T_MAX value
 finds arguable usefulness in the task migration algorithm, by further limiting how much the
 time may increase to achieve a reduction in energy. It is used by the paper to limit the
 baseline algorithms as well, a limitation I chose not to focus on. I thought that letting
 the baseline algorithms produce their results without programmatically restraining the data
 was more useful for analysis (the paper says "Repeat Step1~2 for 10,000 times to find the
 scheduling with the minimum E_total under the constraint that T_total <= T_max." I found in
 practice that this constraint resulted in no schedulings being found as N increased, which
 was inconvenient for my analysis). We can see graphically exactly how these baseline
 algorithms compared to MCC, so focusing on this arbitrary maximum value in the analysis
 seemed less meaningful to me.

--------------------
"sample/"
--------------------

 - the raw data corresponding to a sample run with 13 tasks
    -- "sample13_out.txt" is raw data
    -- "sample13.pdf" is visualization

 - the following bash commands were used to generate data:
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~
         cd code/
         make clean && make DEFINES='-DNUM_TASKS=13' && ./run > ../sample/sample13_out.txt
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~

 - visualization of data was achieved with the following programs:
     -- https://graphviz.gitlab.io/
     -- https://miktex.org/
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~
         cd code/
         python3 schedule_to_tex.py ../sample/sample13_out.txt ../sample/sample13.tex -s
         cd ../sample/
         dot -Tpdf sample13.tex.dot > sample13_tex_dot.pdf
         pdflatex sample13.tex
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~

--------------------
 "code/"
--------------------

 - the code and makefile for the program, basic usage:
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~
         cd code/
         make clean && make
         ./run
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~

 - a python script "schedule_to_tex.py" is given also, for use in visualizing the output
 (warning, it is not a representative of very cleanly written code..) see example usage in
 "sample/" section above

 - The program generates task parameters and a task dependency graph from functions in
 "main.cc". Default parameters for generation are shown in "params.h". Notice that the way
 core execution times are calculated for the random tasks results in a wide difference in
 times across cores. This results in very interesting input cases, where consecutive cores
 use progressively more power but take much less time. For example, task 5 in the
 presentation has the parameters: T_s=2.59198, T_r=3.37373, T_c=9.25091, and
 T_l=[64.6986,30.1758,10.218] (these parameter names come from the research paper). We see
 as a result that the first of the local cores will likely be avoided due to large execution
 times, but interesting tradoff decisions can still result. The parameters of the program
 can be adjusted using preprocessor defines at compile time, but if you wish to change the
 behavior where later cores take significantly less time, you must make your own changes to
 "gen_rand_tasks".

 - To compile the program with different parameters, compile like so:
         ~~~~~~~~~~~~~~~~~~~~~~~~~~~
         cd code/
         make clean
         make DEFINES='-D<NAME>=<VAL> -D<NAME>=<VAL> <etc>'
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~
    where <NAME> is a parameter name, <VAL> is the value of the parameter, and <etc> means to
    repeat the previous pattern for more parameters. The "sample/" section above has an
    example.
        You may adapt the program to take command line inputs of course, or to read a
    parameter file, or etc. I just left it like this for simplicity, despite the
    inconvenience of having to recompile. As a result, `make clean` is necessary every time
    you want to recompile the program with other parameters. The makefile may be adjusted to
    make this more convenient, and precompiled headers may be used to further improve
    compile time if you wish. I didn't bother to optimize these steps, however.
