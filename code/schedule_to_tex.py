#!/bin/python3

import argparse
import random
import sys

SEED = 42
NUM_TASKS = 0
task_graph = []
schedules = []
colors = []
output = ''
dot_output = ''
standalone = False

xoff_dict = {'ws':1.2,'c':.8,'wr':.4}
scale_factor = 16.0

def find_threads(core):
    threads = {'c':[],'wr':[]}
    c = [(t, vals['RT_c'], vals['FT_c']) for t, vals in core.items()]
    wr = [(t, vals['FT_c'], vals['FT_wr']) for t, vals in core.items()]

    c.sort(key=lambda e: e[1])
    wr.sort(key=lambda e: e[1])

    wr_off = 0.4

    threads['wr'].append((wr_off, [wr[0]]))

    for i in range(1, len(wr)):
        found = False
        for t in range(len(threads['wr'])):
            if wr[i][1] >= threads['wr'][t][1][-1][2]:
                found = True
                threads['wr'][t][1].append(wr[i])
                break
        if not found:
            wr_off+=.1
            threads['wr'].append((wr_off, [wr[i]]))

    c_off = wr_off+.4
    threads['c'].append((c_off, [c[0]]))
    for i in range(1, len(c)):
        found = False
        for t in range(len(threads['c'])):
            if c[i][1] >= threads['c'][t][1][-1][2]:
                found = True
                threads['c'][t][1].append(c[i])
                break
        if not found:
            c_off+=.1
            threads['c'].append((c_off, [c[i]]))

    xoff_dict['wr'] = wr_off - (len(threads['wr'])-1)*.05
    xoff_dict['c'] = c_off - (len(threads['c'])-1)*.05
    xoff_dict['ws'] = c_off + .4

    return threads

def finalize_dot():
    with open(dot_output, 'w') as fp:
        fp.write('''digraph {
''')
        for n in range(NUM_TASKS):
            fp.write(str(n)+'''[style=filled fillcolor="'''+colors[n]+'''"];
''')
        for n in range(NUM_TASKS):
            for task in task_graph[n]:
                fp.write(str(n)+''' -> '''+str(task)+'''
''')
        fp.write('''}
''')

def finalize():
    with open(output, 'w') as fp:
        num = 0
        if standalone:
            fp.write('''\\documentclass{article}
\\usepackage[letterpaper, margin=1in]{geometry}
\\usepackage{pgfplots}
\\usepackage[final]{pdfpages}
\\usepackage{xcolor}
''')
            for n in range(NUM_TASKS):
                fp.write('''\\definecolor{t'''+str(n)+'''}{HTML}{'''+colors[n][1:]+'''}
''')
            fp.write('''
\\usetikzlibrary{decorations.pathreplacing, positioning, arrows.meta}
\\pgfplotsset{compat=1.15}
\\newcommand{\\nl}{\\hfill \\break}
\\begin{document}

''')

        fp.write('''\\includepdf[pages=-]{'''+dot_output.replace('..','//').replace('.','_').replace('//','..')+'''.pdf}
''')

        for schedule in schedules:
            t_total = int(schedule['t_total'])+1.0
            fix_factor = t_total/scale_factor
            t_total /= fix_factor
            num+=1
            fp.write('%% Schedule '+str(num))
            fp.write('''
\\begin{center}
\\begin{tikzpicture}[scale=.9]

\\draw[thick, -Triangle] (0,0) -- ('''+str(t_total)+'''cm,0) node[font=\\scriptsize,above left=3pt and -8pt]{};

\\foreach \\x in {'''+str(t_total/16)+''','''+str(t_total/8)+''','''+str(3*t_total/16)+''','''+str(t_total/4)+''',
'''+str(5*t_total/16)+''','''+str(3*t_total/8)+''','''+str(7*t_total/16)+''','''+str(t_total/2)+''','''+str(9*t_total/16)+''',
'''+str(5*t_total/8)+''','''+str(11*t_total/16)+''','''+str(3*t_total/4)+''','''+str(13*t_total/16)+''','''+str(7*t_total/8)+''',
'''+str(15*t_total/16)+'''}
\\draw (\\x cm,3pt) -- (\\x cm,-3pt);

\\foreach \\x/\\descr in {0/0,'''+str(t_total/4)+'''/'''+str(t_total*fix_factor/4)+''','''+str(t_total/2)+'''/'''+str(t_total*fix_factor/2)+''','''+str(3*t_total/4)+'''/'''+str(3*t_total*fix_factor/4)+''','''+str(t_total)+'''/'''+str(t_total*fix_factor)+'''}
\\node[font=\\scriptsize, text height=1.75ex, text depth=.5ex] at (\\x,-.3) {$\\descr$};
''')
            xoff = -.3
            for k, core in schedule.items():
                if k == 't_total' or not core:
                    continue;
                if k != 0:
                    xoff -= .4;
                    fp.write('''
%% Core '''+str(k)+'''
\\foreach \\x/\\col/\\xe/\\xoff in
{''')
                    cnt = 0
                    for t, vals in core.items():
                        cnt+=1
                        fp.write('''
'''+str(vals['RT_l']/fix_factor)+'''/t'''+str(t)+'''/'''+str(vals['FT_l']/fix_factor)+'''/'''+str(xoff-.1)+(''',''' if cnt != len(core.items()) else ''''''))
                    fp.write('''
}
\\filldraw[fill=\\col, line width=1pt]
(\\x,\\xoff) rectangle (\\xe,\\xoff+.2);
\\node[font=\\scriptsize, text height=1.75ex,
text depth=.5ex] at (-.5,'''+str(xoff)+''') {core '''+str(k)+'''};
''')
                else:
                    threads = find_threads(core)

                    for typ in ['ws', 'c', 'wr']:
                        fp.write('''
%% Core '''+str(k)+''' '''+typ+'''
\\foreach \\x/\\col/\\xe/\\xoff in
{''')

                        # TODO

                        if typ == 'ws':
                            cnt = 0
                            for t, vals in core.items():
                                cnt+=1
                                fp.write('''
'''+(str(vals['RT_'+typ]/fix_factor) if typ != 'wr' else str(vals['FT_c']/fix_factor))+'''/t'''+str(t)+'''/'''+str(vals['FT_'+typ]/fix_factor)+'''/'''+str(xoff_dict[typ])+(''',''' if cnt != len(core.items()) else ''''''))

                        else:
                            cnt1 = 0
                            for thread in threads[typ]:
                                cnt1 += 1
                                cnt2 = 0
                                for vals in thread[1]:
                                    cnt2 += 1
                                    fp.write('''
'''+str(vals[1]/fix_factor)+'''/t'''+str(vals[0])+'''/'''+str(vals[2]/fix_factor)+'''/'''+str(thread[0])+(''',''' if (cnt1 != len(threads[typ]) or cnt2 != len(thread[1])) else ''''''))

                        fp.write('''
}
\\filldraw[fill=\\col, line width=1pt]
(\\x,\\xoff) rectangle (\\xe,\\xoff+.2);
\\node[font=\\scriptsize, text height=1.75ex,
text depth=.5ex] at (-.5,'''+str(xoff_dict[typ]+.1)+''') {'''+typ+'''};
''')
            fp.write('''
\\end{tikzpicture}
\\end{center}
\\nl
''')
        if standalone:
            fp.write('''
\\end{document}
''')

    sys.exit(0)


def read_until(fp, str, line=None, end=None):
    if not line:
        line = fp.readline()
    if end:
        if isinstance(end, list):
            stop = False
            while line and str not in line:
                for e in end:
                    if e in line:
                        stop = True
                        break
                if stop:
                    break
                line = fp.readline()
        else:
            while line and str not in line and end not in line:
                line = fp.readline()
    else:
        while line and str not in line:
            line = fp.readline()
    if not line:
        fp.close()
        finalize_dot()
        finalize()
    return line

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='The input filepath.')
    parser.add_argument('output', help='The output filepath.')
    parser.add_argument('-d', '--dot_output', help='The graphviz output filepath.')
    parser.add_argument('-s', '--standalone', action='store_true', help='To generate standalone tex file.')

    args = parser.parse_args()
    output = args.output
    standalone = args.standalone

    if not args.dot_output:
        dot_output = output+'.dot'
    else:
        dot_output = args.dot_output

    random.seed(SEED)

    with open(args.input) as fp:
        line = fp.readline()
        if line:
            line = read_until(fp, 'NUM_TASKS:', line=line)
            NUM_TASKS = int(line.split('NUM_TASKS:')[1].strip())
            colors = ["#"+''.join([random.choice('0123456789ABCDEF') for j in range(6)]) for i in range(NUM_TASKS)]

            line = read_until(fp, 'Task Graph:')
            for n in range(NUM_TASKS):
                line = fp.readline()
                tasks = line.split('task_')
                task_graph.append([])
                for task in tasks[1:]:
                    task_graph[n].append(int(task.split(',')[0]))

        while line:
            line = read_until(fp, 'BEGIN SCHEDULE', line=line)

            schedules.append({})

            line = read_until(fp, 't_total')
            schedules[-1]['t_total'] = float(line.split('=')[1].strip())

            line = read_until(fp, 'BEGIN CORE', end='END')
            while 'END' not in line:
                core = int(line.split('BEGIN CORE')[1].strip())
                schedules[-1][core] = {}

                line = read_until(fp, 'task_', end=['BEGIN','END'])
                while 'task_' in line:
                    task = int(line.split('task_')[1].strip()[:-1])
                    schedules[-1][core][task] = {}
                    line = read_until(fp, 'FT_ws')
                    schedules[-1][core][task]['FT_ws'] = float(line.split('FT_ws=')[1].split(',')[0])
                    schedules[-1][core][task]['FT_wr'] = float(line.split('FT_wr=')[1].split(',')[0])
                    schedules[-1][core][task]['FT_c'] = float(line.split('FT_c=')[1].split(',')[0])
                    schedules[-1][core][task]['FT_l'] = float(line.split('FT_l=')[1].strip())
                    line = read_until(fp, 'RT_ws')
                    schedules[-1][core][task]['RT_ws'] = float(line.split('RT_ws=')[1].split(',')[0])
                    schedules[-1][core][task]['RT_c'] = float(line.split('RT_c=')[1].split(',')[0])
                    schedules[-1][core][task]['RT_l'] = float(line.split('RT_l=')[1].strip())

                    line = read_until(fp, 'task_', end=['BEGIN','END'])

                line = read_until(fp, 'BEGIN CORE', line=line, end='END')
