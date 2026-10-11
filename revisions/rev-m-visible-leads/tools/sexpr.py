import re,json
class Atom(str):pass
def parse(s):
 toks=re.findall(r'\(|\)|"(?:\\.|[^"\\])*"|[^\s()]+',s);stack=[];root=None
 for t in toks:
  if t=='(':a=[];stack.append(a)
  elif t==')':
   a=stack.pop()
   if stack:stack[-1].append(a)
   else:root=a
  else:stack[-1].append(json.loads(t) if t.startswith('"') else Atom(t))
 assert not stack and root is not None, "Unbalanced s-expression"
 return root
def dump(a):
 if isinstance(a,list):return '('+' '.join(dump(x) for x in a)+')'
 if isinstance(a,Atom):return a
 return json.dumps(a,ensure_ascii=False)
def get(a,k):return next((x for x in a if isinstance(x,list) and x and x[0]==k),None)
def allof(a,k):return [x for x in a if isinstance(x,list) and x and x[0]==k]
def prop(a,k):return next((x for x in allof(a,'property') if x[1]==k),None)
