def make_graph(grammar):
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
    # import pprint
    #
    # pprint.pprint(grammar.symbol2label)
    # pprint.pprint(grammar.dfas)
    #
    # pprint.pprint(grammar.labels)
    #
    # pprint.pprint(grammar.keywords)
    # pprint.pprint(grammar.tokens)

    nodes = []
    edges = []

    for dfaindex, dfa_elem in enumerate(grammar.dfas.items()):
        symbol, (dfa, first_sets) = dfa_elem
        nodes.append((symbol, grammar.number2symbol[symbol]))

        # iterate over  in dfa

        for x in first_sets:
            edges.append((symbol, x))

    for v, k in grammar.keywords.items():
        if (k, v) not in nodes:
            nodes.append((k, v))

    section1 = ""
    for node in nodes:
        id, name = node
        section1 += "{" + "id: {0}, label: '{1}', shape: 'box', color:'#AAC2FC'".format(id, name) + "},"

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
