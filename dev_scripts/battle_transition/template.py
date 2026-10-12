""" Writes a skeleton of one new battle transition type """
import sys
import os

def insert_into_file(
        filename,
        line_to_insert,
        position_search_start_line,
        insert_before_line,
):
    with open(filename, 'r', encoding="utf-8") as f:
        data_str = f.read()
    data_lines = data_str.splitlines()
    position_search_start = data_lines.index(position_search_start_line)
    insert_before = data_lines.index(insert_before_line, position_search_start)
    while '' == data_lines[insert_before - 1].strip():
        insert_before = insert_before - 1
    data_lines[insert_before:0] = [line_to_insert]

    with open(filename, 'w', encoding="utf-8") as f:
        f.write('\n'.join(data_lines))
        f.write('\n')

def main(space_name, into_yoshord_space):
    snake_name = space_name.replace(' ', '_')
    upper_name = snake_name.upper()
    title_name = space_name.title().replace(' ', '')

    constant = 'B_TRANSITION_' + upper_name

    insert_into_file(
        'include/battle_transition.h',
        '    ' + constant + ',',
        'enum BattleTransition {',
        '    // end yoshord transitions' if into_yoshord_space else '    B_TRANSITION_COUNT',
    )

    insert_into_file(
        'src/battle_transition.c',
        '    [' + constant + '] = &Task_Intro,',
        'static const TaskFunc sTasks_Intro[B_TRANSITION_COUNT] =',
        '    // end yoshord transitions' if into_yoshord_space else '};',
    )

    insert_into_file(
        'src/battle_transition.c',
        '    [' + constant + '] = &Task_' + title_name + ',',
        'static const TaskFunc sTasks_Main[B_TRANSITION_COUNT] =',
        '    // end yoshord transitions' if into_yoshord_space else '};',
    )

    if into_yoshord_space:
        insert_into_file(
            'include/battle_transition_yoshord.h',
            'void Task_' + title_name + '(u8 taskId);',
            '#define GUARD_BATTLE_TRANSITION_YOSHORD_H',
            '#endif',
        )

    with open('src/battle_transition/' + snake_name + '.c', 'w', encoding="utf-8") as f:
        f.write(
            '#include "global.h"\n'
            '#include "battle_transition.h"\n'
            '#include "battle_transition_shared.h"\n'
        )
        if into_yoshord_space:
            f.write('#include "battle_transition_yoshord.h"\n')
        f.write(
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
        main(sys.argv[1], False if len(sys.argv) < 3 else sys.argv[2] == '-y')
