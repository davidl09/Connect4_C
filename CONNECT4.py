height = 6
width = 7
EMPTY = 0

def iswin(board, player):
    if list_legal_moves(board) == []:
        return 'draw'
    for i in range(height):  #check rows, works
        for j in range(width - 3):
            if all(board[i][j+k] == player for k in range(4)):
                return [(i, j+k) for k in range(4)]

    for j in range(width):
        for i in range(height - 3):
            if all(board[i+k][j] == player for k in range(4)):
                return [(i+k, j) for k in range(4)]

    
    for i in range(height - 3):  #check upward diagonals
        for j in range(width - 3):
            if all(board[i+k][j+k] == player for k in range(4)):
                return [(i+k, j+k) for k in range(4)]
    
    for i in range(3, height):  #check upward diagonals
        for j in range(width - 3):
            if all(board[i-k][j+k] == player for k in range(4)):
                return [(i-k, j+k) for k in range(4)]

def list_legal_moves(board):
    moves = []
    for col in range(width):
        if board[0][col] == 0:
            moves.append(col)
    return moves





