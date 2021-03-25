import plotly.express as px
fig = px.bar(x=["a", "a", "b", 3], y = [1,2,3,4])
fig.update_xaxes(type='category')
fig.show()