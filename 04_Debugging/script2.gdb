set pagination off
set confirm off
set print pretty on
b should_print if ((idx+1) >= 28) && ((idx+1) <= 35)
commands
  printf "@@@ start=%d stop=%d step=%d current=%d idx=%d\n", start, stop, step, current, idx
  cont
end
run -100 100 3 >/dev/null
quit
