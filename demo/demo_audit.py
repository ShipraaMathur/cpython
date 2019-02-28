def auditor(event, *args):
    # Audit system events.

    if event == 'subprocess.popen':
        cmd, shell = args
        print(f"Executing {cmd} with shell={shell}")

    elif event == 'urllib.Request':
        print(f"Request made to fetch URL {args}")
        raise RuntimeError("Denied!!")

    elif event == 'socket.bind':
        raise RuntimeError("Nope.")

# install the auditor
import sys
sys.addaudithook(auditor)
print("Installed demo auditor.")