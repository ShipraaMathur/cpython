def make_graph(grammar, tokens):
    header = """
    <!doctype html>
    <html>
    <head>
      <title>CPython DFA Graph</title>
      <script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/vis/4.21.0/vis.min.js"></script>
      <link href="https://cdnjs.cloudflare.com/ajax/libs/vis/4.21.0/vis.min.css" rel="stylesheet" type="text/css" />
      <style type="text/css">
        #mynetwork {
          width: 1200px;
          height: 800px;
          border: 1px solid lightgray;
        }
        p {
          max-width:1200px;
        }
      </style>
    </head>
    <body>
    <div id="mynetwork"></div>
    <script type="text/javascript">
      // create an array with nodes
      var nodes = new vis.DataSet([
    """

    nodes = {}
    edges = []

    for label, k in tokens.items():
        if k != 256:
            nodes[k] = (label, 'red')

    for dfaindex, dfa_elem in enumerate(grammar.dfas.items()):
        symbol, (dfa, first_sets) = dfa_elem
        if symbol not in nodes:
            nodes[symbol] = (grammar.number2symbol[symbol], 'green')

        for edge in dfa:
            for a, b in edge:
                if (a, b) not in edges:
                    edges.append((a, b))

        # next states in dfa
        for x in first_sets:
            edges.append((symbol, x))

    for v, k in grammar.keywords.items():
        if k not in nodes:
            nodes[k] = (v, 'blue')

    section1 = ""
    for id, node in nodes.items():
        name, color = node
        section1 += "{" + "id: {0}, label: '{1}', shape: 'box', color:'{2}'".format(id, name, color) + "},"

    section2 = """
      ]);
      var edges = new vis.DataSet([
    """
    section3 = ""
    for edge in edges:
        a, b = edge
        section3 += "{" + "from: {0}, to: {1}".format(a, b) + ", color:{color:'red'}},"

    footer = """
      ]);
      var container = document.getElementById('mynetwork');
      var data = {
        nodes: nodes,
        edges: edges
      };
      var options = {
        nodes: {
          shape: 'circle'
        }
      };
      var network = new vis.Network(container, data, options);
    </script>
    </body>
    </html>
    """
    with open("out.html", "w") as output:
        output.write(header+section1+section2+section3+footer)
    print("Created out.html")
