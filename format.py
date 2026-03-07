import sys
import re

def main():
    for filepath in sys.argv[1:]:
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                content = f.read()

            # Replacing simple ` {` at the end of the line
            # We'll use re.sub with multiline to match whitespace followed by { at end of line.
            # E.g. anything ending in ` {` or `) {` etc.
            
            lines = content.split('\n')
            new_lines = []
            
            for line in lines:
                # remove trailing spaces, carriage returns
                line = line.rstrip()
                if line.endswith('{') and len(line.strip()) > 1:
                    # separate code from '{'
                    prefix = line[:-1].rstrip()
                    # keep original leading whitespace for the brace
                    indent = len(line) - len(line.lstrip())
                    brace_indent = line[:indent]
                    
                    new_lines.append(prefix)
                    new_lines.append(brace_indent + '{')
                else:
                    new_lines.append(line)
                    
            # second pass: convert 2 spaces to 1 tab everywhere at the beginning of lines
            final_lines = []
            for line in new_lines:
                match = re.match(r'^( +)(.*)$', line)
                if match:
                    spaces = match.group(1)
                    rest = match.group(2)
                    num_tabs = len(spaces) // 2
                    rem_spaces = len(spaces) % 2
                    final_lines.append('\t' * num_tabs + ' ' * rem_spaces + rest)
                else:
                    final_lines.append(line)
            
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write('\n'.join(final_lines))
            print(f"Formatted {filepath}")
        except Exception as e:
            print(f"Error {filepath}: {e}")

if __name__ == "__main__":
    main()
