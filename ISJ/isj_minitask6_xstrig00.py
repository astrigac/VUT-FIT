class Point:
    def __init__(self, x = 0, y = 0):
        self.x = x
        self.y = y
        
    def __str__(self):
        message = f"Point({self.x},{self.y})"
        return message
    
    def __sub__(self, other):
        new_x = self.x - other.x
        new_y = self.y - other.y
        return Point(new_x, new_y)

p0 = Point()
print(p0) # should be: Point(0,0)
p1 = Point(3, 4)
result = p0-p1
print(result) # should be: Point(-3,-4)
