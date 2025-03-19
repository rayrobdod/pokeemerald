""" Writes a skeleton of one new battle transition type """
import sys
import os

def append_include(directory, included_filename):
    a = directory.split('/')
    a.pop(0)
    directory_without_first_level = '/'.join(a)

    with open(directory + '/_index.h', 'r', encoding="utf-8") as f:
        data_str = f.read()
    data_lines = data_str.splitlines()
    data_lines.append('#include "' + included_filename + '"')
    out_lines = sorted(list(set(data_lines)))
    out_str = '\n'.join(out_lines) + '\n'
    with open(directory + '/_index.h', 'w', encoding="utf-8") as f:
        f.write(out_str)


def main(space_name):
    snake_name = space_name.replace(' ', '_')
    upper_name = snake_name.upper()
    title_name = space_name.title().replace(' ', '')

    constant = 'B_TRANSITION_' + upper_name

    append_include('include/battle_transition_tasks', 'battle_transition_tasks/' + snake_name + '.h')
    with open('include/battle_transition_tasks/' + snake_name + '.h', 'w', encoding="utf-8") as f:
        f.write(
            '#ifndef GUARD_BATTLE_TRANSITION_' + upper_name + '_H\n'
            '#define GUARD_BATTLE_TRANSITION_' + upper_name + '_H\n'
            '\n'
            'void Task_' + title_name + '(u8 taskId);\n'
            '\n'
            '#endif // GUARD_BATTLE_TRANSITION_' + upper_name + '_H\n'
        )

    append_include('include/constants/battle_transition', 'constants/battle_transition/' + snake_name + '.h')
    with open('include/constants/battle_transition/' + snake_name + '.h', 'w', encoding="utf-8") as f:
        f.write('    ' + constant + ',\n')

    append_include('src/battle_transition_tasks_intro', snake_name + '.h')
    with open('src/battle_transition_tasks_intro/' + snake_name + '.h', 'w', encoding="utf-8") as f:
        f.write('    [' + constant + '] = &Task_Intro,\n')

    append_include('src/battle_transition_tasks_main', snake_name + '.h')
    with open('src/battle_transition_tasks_main/' + snake_name + '.h', 'w', encoding="utf-8") as f:
        f.write('    [' + constant + '] = &Task_' + title_name + ',\n')

    with open('src/battle_transition/' + snake_name + '.c', 'w', encoding="utf-8") as f:
        f.write(
            '#include "global.h"\n'
            '#include "battle_transition.h"\n'
            '#include "battle_transition_shared.h"\n'
            '#include "battle_transition_tasks/' + snake_name + '.h"\n'
            '#include "scanline_effect.h"\n'
            '#include "task.h"\n'
            '\n'
            'static bool8 ' + title_name + '_Init(struct Task *);\n'
            'static bool8 ' + title_name + '_Main(struct Task *);\n'
            '\n'
            'static const TransitionStateFunc s' + title_name + '_Funcs[] = {\n'
            '    ' + title_name + '_Init,\n'
            '    ' + title_name + '_Main,\n'
            '};\n'
            '\n'
            'void Task_' + title_name + '(u8 taskId)\n'
            '{\n'
            '    while (s' + title_name + '_Funcs[gTasks[taskId].tState](&gTasks[taskId]));\n'
            '}\n'
            '\n'
            'static bool8 ' + title_name + '_Init(struct Task *task)\n'
            '{\n'
            '    InitTransitionData();\n'
            '    ScanlineEffect_Clear();\n'
            '\n'
            '    task->tState++;\n'
            '    return TRUE;\n'
            '}\n'
            '\n'
            'static bool8 ' + title_name + '_Main(struct Task *task)\n'
            '{\n'
            '    return FALSE;\n'
            '}\n'
        )


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('template.py "space case name"')
    else:
        main(sys.argv[1])
