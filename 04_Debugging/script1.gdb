set pagination off
set confirm off
set print pretty on
b should_print if (current % 5) == 0
commands
  printf "@@@ start=%d stop=%d step=%d current=%d idx=%d\n", start, stop, step, current, idx
  cont
end
run 1 12 >/dev/null
quit
