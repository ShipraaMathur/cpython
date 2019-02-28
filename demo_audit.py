def auditor(event, *args):
    if event == 'exec':
        print("Attempting to run code in isolation")