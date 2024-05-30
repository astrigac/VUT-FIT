#!/usr/bin/env python3

class Polynomial:
    """
    Polynomial class represents and manipulates polynomials.
    """
    
    def __init__(self, *params, **xinit):
        self.value_list = [0]  # initialize the list to avoid starting with a string
        key0 = False  # defines if the first value was changed
        if params:
            if isinstance(params[0], list):  # polynomial as list
                self.value_list = params[0]
            else:  # polynomial as numbers
                self.value_list = []
                for value in params:
                    if isinstance(value, int):
                        self.value_list.append(value)
                    else:
                        raise TypeError(f"Expected {int}, not {type(value)}")
        
        if xinit:
            for key, value in xinit.items():
                if value != 0:
                    new_key = int(key.replace("x", ""))
                    if new_key == 0:
                        key0 = True
                    # Resizing list
                    while len(self.value_list) <= new_key:
                        self.value_list.append(0)
                    self.value_list[new_key] = int(value)
            if not key0 and self.value_list:
                self.value_list[0] = 0
        self._format()

    def _format(self):
        """ Format vector to minimal length. """
        while len(self.value_list) > 1 and self.value_list[-1] == 0:
            self.value_list.pop()
        if not self.value_list:
            self.value_list.append(0)

    def __str__(self):
        """ Used for printing polynomial in basic format. """
        self._format()
        if self.value_list == [0]:
            return "0"
        terms = []
        for index, value in enumerate(self.value_list):
            if value != 0:
                sign = " - " if value < 0 else " + "
                number = abs(value)
                if index == 0:
                    term = f"{number}"
                elif index == 1:
                    term = f"{'' if number == 1 else number}x"
                else:
                    term = f"{'' if number == 1 else number}x^{index}"
                terms.append(sign.strip() + ' ' + term)
        terms.reverse()  # Reverse the list to start from the highest order
        result = " ".join(terms).strip()
        if result.startswith("+"):
            result = result[2:]  # Remove the leading '+ ' for a cleaner output
        return result

    def __eq__(self, other):
        """ Checks if two polynomials are equal. """
        return isinstance(other, Polynomial) and self.value_list == other.value_list

    def __add__(self, other):
        """ Addition, doesn't change values of original polynomial. """
        if isinstance(other, Polynomial):
            other._format()
            self._format()
            if len(other.value_list) > len(self.value_list):
                result = list(other.value_list)
            else:
                result = list(self.value_list)
            
            for index, value in enumerate(min(other.value_list, self.value_list, key=len)):
                result[index] += value
            
            return Polynomial(result)
        return NotImplemented

    def __sub__(self, other):
        """ Subtraction of two polynomials. """
        max_len = max(len(self.value_list), len(other.value_list))
        result = [0] * max_len
        for i in range(max_len):
            val1 = self.value_list[i] if i < len(self.value_list) else 0
            val2 = other.value_list[i] if i < len(other.value_list) else 0
            result[i] = val1 - val2
        return Polynomial(result)

    def __mul__(self, other):
        """ Polynomial multiplication. """
        result = [0] * (len(self.value_list) + len(other.value_list) - 1)
        for i in range(len(self.value_list)):
            for j in range(len(other.value_list)):
                result[i + j] += self.value_list[i] * other.value_list[j]
        return Polynomial(result)

    def __pow__(self, power):
        """ Raise polynomial to power. """
        if not isinstance(power, int) or power < 0:
            raise ValueError("Power must be a non-negative integer")
        result = Polynomial([1])
        for _ in range(power):
            result = result * self
        return result

    def derivative(self):
        """ Returns the first derivative of the polynomial. """
        if len(self.value_list) <= 1:
            return Polynomial([0])  # The derivative of a constant is zero.
        result = list(self.value_list[1:])  # Copy starting from the second element
        for index in range(len(result)):
            result[index] *= (index + 1)
            
        return Polynomial(result)

    def at_value(self, value1, *value2):
        """ Evaluate the polynomial at one or two values. """
        result = sum(coef * (value1 ** idx) for idx, coef in enumerate(self.value_list))
        if value2:
            result2 = sum(coef * (value2[0] ** idx) for idx, coef in enumerate(self.value_list))
            return result2 - result
        return result


# Function to conduct unit tests for the Polynomial class
def test():
    """Run test cases."""
    assert str(Polynomial(0,1,0,-1,4,-2,0,1,3,0)) == "3x^8 + x^7 - 2x^5 + 4x^4 - x^3 + x"
    assert str(Polynomial([-5,1,0,-1,4,-2,0,1,3,0])) == "3x^8 + x^7 - 2x^5 + 4x^4 - x^3 + x - 5"
    assert str(Polynomial(x7=1, x4=4, x8=3, x9=0, x0=0, x5=-2, x3=-1, x1=1)) == "3x^8 + x^7 - 2x^5 + 4x^4 - x^3 + x"
    assert str(Polynomial(x2=0)) == "0"
    assert str(Polynomial(x0=0)) == "0"
    assert Polynomial(x0=2, x1=0, x3=0, x2=3) == Polynomial(2,0,3)
    assert Polynomial(x2=0) == Polynomial(x0=0)
    assert str(Polynomial(x0=1)+Polynomial(x1=1)) == "x + 1"
    assert str(Polynomial([-1,1,1,0])+Polynomial(1,-1,1)) == "2x^2"
    pol1 = Polynomial(x2=3, x0=1)
    pol2 = Polynomial(x1=1, x3=0)
    assert str(pol1+pol2) == "3x^2 + x + 1"
    assert str(Polynomial(x0=-1,x1=1)**1) == "x - 1"
    assert str(Polynomial(x0=-1,x1=1)**2) == "x^2 - 2x + 1"
    pol3 = Polynomial(x0=-1,x1=1)
    assert str(pol3**4) == "x^4 - 4x^3 + 6x^2 - 4x + 1"
    assert str(Polynomial(x0=2).derivative()) == "0"
    assert str(Polynomial(x3=2,x1=3,x0=2).derivative()) == "6x^2 + 3"
    assert str(Polynomial(x3=2,x1=3,x0=2).derivative().derivative()) == "12x"
    pol4 = Polynomial(x3=2,x1=3,x0=2)
    assert str(pol4.derivative()) == "6x^2 + 3"
    assert Polynomial(-2,3,4,-5).at_value(0) == -2
    assert Polynomial(x2=3, x0=-1, x1=-2).at_value(3) == 20
    assert Polynomial(x2=3, x0=-1, x1=-2).at_value(3,5) == 44
    pol5 = Polynomial([1,0,-2])
    assert pol5.at_value(-2.4) == -10.52
    assert pol5.at_value(-1,3.6) == -23.92

if __name__ == '__main__':
    test()
