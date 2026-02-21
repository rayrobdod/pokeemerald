
WORDARRAY DECOMPRESS_FUNCTION_NAME(struct ByteArray src, enum DecompressVerbosity disassemble)
{
    WORDARRAY dest;

    dest.size = ((src.buffer[3] << 16) | (src.buffer[2] << 8) | src.buffer[1]) / sizeof(WORD);
    dest.buffer = malloc(dest.size * sizeof(WORD));

    if (dest.buffer == NULL)
        FATAL_ERROR("Fatal error while decompressing Frit file.\n");

    if (disassemble)
        printf("SIZE: %zd\n", dest.size);

    WORD regs[NUM_REGS] = {0};

    size_t srcPos = 4;
    size_t destPos = 0;

    while (srcPos < src.size)
    {
        const size_t initialSrcPos = srcPos;
        unsigned op = src.buffer[srcPos++];
        unsigned op_code = op >> 6;

        if (0 == op_code)
        {
            unsigned from = (op & 0x30) >> 4;
            unsigned to = (op & 0xC) >> 2;
            unsigned hi = (op & 0x2);
            unsigned low = (op & 0x1);
            unsigned hiValue = 0;
            unsigned lowValue = 0;
            if (hi)
            {
                if (srcPos >= src.size)
                    FATAL_ERROR("Fatal error while decompressing Frit file.\n");

                hiValue = src.buffer[srcPos++] << 8;
            }
            if (low)
            {
                if (srcPos >= src.size)
                    FATAL_ERROR("Fatal error while decompressing Frit file.\n");

                lowValue = src.buffer[srcPos++];
            }
            unsigned operand = hiValue | lowValue;

            if (disassemble)
            {
                char hiStr[8] = "N/A";
                char lowStr[8] = "N/A";
                if (hi)
                    snprintf(hiStr, sizeof(hiStr), "%02x", hiValue >> 8);
                if (low)
                    snprintf(lowStr, sizeof(lowStr), "%02x", lowValue);

                printf("  %04x %04x %04x %04x  %8zd %8zd | XOR   %d->%d   %3s %3s\n",
                    regs[0], regs[1], regs[2], regs[3], initialSrcPos, destPos, from, to, hiStr, lowStr);
            }

            regs[to] = regs[from] ^ operand;
        }
        else
        {
            signed delta = op_code - 2;

            unsigned reg = (op & 0x30) >> 4;
            unsigned length = (op & 0x0F) + 1;
            if (length == 0x10)
                length = src.buffer[srcPos++] + 31;

            if (disassemble)
            {
                char deltaStr = (delta == 0 ? '0' : delta == 1 ? '+' : '-');

                printf("  %04x %04x %04x %04x  %8zd %8zd | RUN%c  %d    %3d\n",
                    regs[0], regs[1], regs[2], regs[3], initialSrcPos, destPos, deltaStr, reg, length);
            }

            if (destPos + length > dest.size)
                FATAL_ERROR("Fatal error while decompressing Frit file.\n");

            for (unsigned i = 0; i < length; i++, destPos++)
            {
                dest.buffer[destPos] = regs[reg];
                regs[reg] += delta;
            }
        }
    }

    if (disassemble)
        printf("  %04x %04x %04x %04x  %8zd %8zd | END\n\n",
            regs[0], regs[1], regs[2], regs[3], srcPos, destPos);

    return dest;
}

struct ByteArray COMPRESS_FUNCTION_NAME(WORDARRAY src) {
    struct RunArray runs = {
        .size = 0,
        .buffer = calloc(sizeof(struct Run), src.size),
    };

    size_t srcPos = 0;

    while (srcPos < src.size)
    {
        char delta = 0;
        size_t length = 1;
        const WORD start = src.buffer[srcPos];
        srcPos++;

        if (srcPos < src.size)
        {
            WORD current = src.buffer[srcPos];

            if (current - start == 0)
            {
                while (srcPos < src.size && src.buffer[srcPos] == start)
                {
                    ++srcPos;
                    ++length;
                }
            }
            else if (current - start == 1)
            {
                delta = 1;
                while (srcPos < src.size && src.buffer[srcPos] == current)
                {
                    ++srcPos;
                    ++length;
                    ++current;
                }
            }
            else if (current - start == -1)
            {
                delta = -1;
                while (srcPos < src.size && src.buffer[srcPos] == current)
                {
                    ++srcPos;
                    ++length;
                    --current;
                }
            }
        }

        runs.buffer[runs.size].delta = delta;
        runs.buffer[runs.size].length = length;
        runs.buffer[runs.size].start = start;
        runs.size++;
    }

    /* run optimization pass */
    for (size_t runPos = 0; runPos < runs.size; runPos++)
    {
        // The delta does not change the immediate output of a 1-length run,
        // but changing the delta changes the register value after the run,
        // and a different delta can better set up registers for future runs
        if (1 == runs.buffer[runPos].length)
        {
            size_t next_Zero	= find_run_with_start(runs, runPos + 1, runs.buffer[runPos].start + 0);
            size_t next_Pos	= find_run_with_start(runs, runPos + 1, runs.buffer[runPos].start + 1);
            size_t next_Neg	= find_run_with_start(runs, runPos + 1, runs.buffer[runPos].start - 1);

            if (next_Pos < next_Neg && next_Pos < next_Zero)
            {
                runs.buffer[runPos].delta = 1;
            }
            if (next_Neg < next_Pos && next_Neg < next_Zero)
            {
                runs.buffer[runPos].delta = -1;
            }
        }

        if (runPos + 1 < runs.size &&
            0 == runs.buffer[runPos].delta &&
            1 < runs.buffer[runPos].length &&
            0 != runs.buffer[runPos + 1].delta &&
            runs.buffer[runPos].start + runs.buffer[runPos + 1].delta == runs.buffer[runPos + 1].start &&
            true)
        {
                runs.buffer[runPos].length -= 1;
                runs.buffer[runPos+1].length += 1;
                runs.buffer[runPos+1].start -= runs.buffer[runPos+1].delta;
        }

        if (runPos + 1 < runs.size &&
            0 != runs.buffer[runPos].delta &&
            1 < runs.buffer[runPos].length &&
            0 == runs.buffer[runPos + 1].delta &&
            run_last(runs.buffer[runPos]) == runs.buffer[runPos + 1].start &&
            true)
        {
                runs.buffer[runPos].length -= 1;
                runs.buffer[runPos+1].length += 1;
        }
    }

    size_t longest_run = 0;
    for (size_t runPos = 0; runPos < runs.size; runPos++)
    {
        if (longest_run < runs.buffer[runPos+1].length)
            longest_run = runs.buffer[runPos+1].length;
    }

    struct ByteArray dest = {
        .size = 4,
        .buffer = calloc(sizeof(WORD), runs.size * (5 + 2 * longest_run / LONGEST_RUN) + 4),
    };

    unsigned size_in_bytes = src.size * sizeof(WORD);
    dest.buffer[0] = _Generic(
        (WORD) 1,
            unsigned short :
                0x42,
            unsigned char :
                0x41
        );
    dest.buffer[1] = size_in_bytes;
    dest.buffer[2] = size_in_bytes >> 8;
    dest.buffer[3] = size_in_bytes >> 16;

    WORD regs[NUM_REGS] = {0};

    for (size_t run_it = 0; run_it < runs.size; run_it++)
    {
        struct Run run = runs.buffer[run_it];

        unsigned runReg;
        // use a register that already has the correct start value
        for (runReg = 0; runReg < NUM_REGS; runReg++)
        {
            if (run.start == regs[runReg])
                break;
        }

        // Otherwise, if two registers have the same value,
        // use one of those two
        if (runReg >= NUM_REGS)
        {
            for (int i = 0; i < NUM_REGS; i++)
            for (int j = i + 1; j < NUM_REGS; j++)
            {
                if (regs[i] == regs[j])
                    runReg = j;
            }
        }

        // Otherwise, use the register whose value will be needed least soon,
        // or whose value will not be needed at all
        if (runReg >= NUM_REGS)
        {
            size_t runRegNextUse = run_it;

            for (unsigned i = 0; i < NUM_REGS; i++)
            {
                size_t iNextUse = find_run_with_start(runs, run_it, regs[i]);
                if (iNextUse > runRegNextUse)
                {
                    runRegNextUse = iNextUse;
                    runReg = i;
                }
            }
        }

        if (runReg >= NUM_REGS)
            runReg = 0;

        if (run.start != regs[runReg])
        {
            unsigned fromReg = runReg;
            for (unsigned i = 0; i < NUM_REGS; i++)
            {
                if ((run.start & 0xFF) == (regs[i] & 0xFF))
                {
                    fromReg = i;
                    break;
                }
                if ((run.start & 0xFF00) == (regs[i] & 0xFF00))
                {
                    fromReg = i;
                    break;
                }
            }

            unsigned hi = (run.start & 0xFF00) ^ (regs[fromReg] & 0xFF00);
            unsigned low = (run.start & 0xFF) ^ (regs[fromReg] & 0xFF);

            dest.buffer[dest.size++] = (fromReg << 4) | (runReg << 2) | (hi ? 2 : 0) | (low ? 1 : 0);
            if (hi)
                dest.buffer[dest.size++] = hi >> 8;
            if (low)
                dest.buffer[dest.size++] = low;
        }

        char opcode = (2 + run.delta) & 0x3;
        unsigned length = run.length;

        while (length > LONGEST_RUN)
        {
                dest.buffer[dest.size++] = (opcode << 6) | (runReg << 4) | 0xF;
                dest.buffer[dest.size++] = 0xFF;
                length -= LONGEST_RUN;
        }

        unsigned lengthSub1 = length - 1;

        if (lengthSub1 < 15)
        {
            dest.buffer[dest.size++] = (opcode << 6) | (runReg << 4) | lengthSub1;
        }
        else
        {
            unsigned lengthSub16 = lengthSub1 - 15;

            if (lengthSub16 < 15)
            {
                dest.buffer[dest.size++] = (opcode << 6) | (runReg << 4) | 0xE;
                dest.buffer[dest.size++] = (opcode << 6) | (runReg << 4) | lengthSub16;
            }
            else
            {
                unsigned lengthSub31 = lengthSub16 - 15;
                dest.buffer[dest.size++] = (opcode << 6) | (runReg << 4) | 0xF;
                dest.buffer[dest.size++] = lengthSub31;
            }
        }

        regs[runReg] = run_end(run);
    }

    while (dest.size % 4 != 0)
        dest.buffer[dest.size++] = 0;

    free(runs.buffer);
    return dest;
}
