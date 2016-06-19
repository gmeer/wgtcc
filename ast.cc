#include "ast.h"
#include "error.h"
#include "visitor.h"


/*
 * accept
 */

void EmptyStmt::Accept(Visitor* v) {
    v->VisitEmptyStmt(this);
}

void LabelStmt::Accept(Visitor* v) {
    v->VisitLabelStmt(this);
}

void IfStmt::Accept(Visitor* v) {
    v->VisitIfStmt(this);
}

void JumpStmt::Accept(Visitor* v) {
    v->VisitJumpStmt(this);
}

void ReturnStmt::Accept(Visitor* v) {
    v->VisitReturnStmt(this);
}

void CompoundStmt::Accept(Visitor* v) {
    v->VisitCompoundStmt(this);
}

void BinaryOp::Accept(Visitor* v) {
    v->VisitBinaryOp(this);
}

void UnaryOp::Accept(Visitor* v) {
    v->VisitUnaryOp(this);
}

void ConditionalOp::Accept(Visitor* v) {
    v->VisitConditionalOp(this);
}

void FuncCall::Accept(Visitor* v) { 
    v->VisitFuncCall(this);
}

void Variable::Accept(Visitor* v) {
    v->VisitVariable(this);
}

void Constant::Accept(Visitor* v) {
    v->VisitConstant(this);
}

void TempVar::Accept(Visitor* v) {
    v->VisitTempVar(this);
}

void FuncDef::Accept(Visitor* v) {
    v->VisitFuncDef(this);
}

void TranslationUnit::Accept(Visitor* v) {
    v->VisitTranslationUnit(this);
}

/*
 * Expression
 */

ConditionalOp* ConditionalOp::TypeChecking(void)
{

    //TODO: type checking

    //TODO: type evaluation

    return this;
}

// TODO(wgtdkp):
bool BinaryOp::EvaluateConstant(Constant* cons)
{
    //bool res = true;
    switch (_op) {
    case '+':
        // TODO(wgtdkp):
        //res &= _lhs->EvaluateConstant(cons);
        //res &= _rhs->EvaluateConstant(cons);
        //if (res) {
        //    if (_ty->Is)
        //}
    case '-':
    case '*':
    case '/':
    case '%':
    case '<':
    case '>':
    case '|':
    case '&':
    case '^':
    case Token::LEFT_OP:
    case Token::RIGHT_OP:
    case Token::AND_OP:
    case Token::OR_OP:
        // TODO(wgtdkp):
        //if (_lhs->EvaluateConstant(cons) 
        //        && _rhs->EvaluateConstant(cons)) {
        //    
        //    return true;
        //}
        return false;

    // N1548 6.6 [3]
    // "Constant expressions shall not contain assignment"
    case '=':
    case ',': // TOOD(wgtdkp): Not simple as such
        return false;
    default:
        assert(0);
    }
}

BinaryOp* BinaryOp::TypeChecking(void)
{
    switch (_op) {
    case '[':
        return SubScriptingOpTypeChecking();

    case '*':
    case '/':
    case '%':
        return MultiOpTypeChecking();

    case '+':
    case '-':
        return AdditiveOpTypeChecking();

    case Token::LEFT_OP:
    case Token::RIGHT_OP:
        return ShiftOpTypeChecking();

    case '<':
    case '>':
    case Token::LE_OP:
    case Token::GE_OP:
        return RelationalOpTypeChecking();

    case Token::EQ_OP:
    case Token::NE_OP:
        return EqualityOpTypeChecking();

    case '&':
    case '^':
    case '|':
        return BitwiseOpTypeChecking();

    case Token::AND_OP:
    case Token::OR_OP:
        return LogicalOpTypeChecking();

    case '=':
        return AssignOpTypeChecking();

    default:
        assert(0);
    }
    
    return nullptr; //make compiler happy
}

BinaryOp* BinaryOp::SubScriptingOpTypeChecking(void)
{
    auto lhsType = _lhs->Ty()->ToPointerType();
    if (nullptr == lhsType)
        Error("an pointer expected");
    if (!_rhs->Ty()->IsInteger())
        Error("the operand of [] should be intger");

    //the type of [] operator is the type the pointer pointed to
    _ty = lhsType->Derived();
    return this;
}

BinaryOp* BinaryOp::MemberRefOpTypeChecking(const char* rhsName)
{
    StructUnionType* structUnionType;
    if (Token::PTR_OP == _op) {
        auto pointer = _lhs->Ty()->ToPointerType();
        if (pointer == nullptr) {
            Error("pointer expected for operator '->'");
        } else {
            structUnionType = pointer->Derived()->ToStructUnionType();
            if (structUnionType == nullptr)
                Error("pointer to struct/union expected");
        }
    } else {
        structUnionType = _lhs->Ty()->ToStructUnionType();
        if (nullptr == structUnionType)
            Error("an struct/union expected");
    }

    if (nullptr == structUnionType)
        return this; //the _rhs is lefted nullptr

    _rhs = structUnionType->Find(rhsName);
    if (nullptr == _rhs) {
        Error("'%s' is not a member of '%s'", rhsName, "[obj]");
    } else {
        _ty = _rhs->Ty();
    }

    return this;
}

BinaryOp* BinaryOp::MultiOpTypeChecking(void)
{
    auto lhsType = _lhs->Ty()->ToArithmType();
    auto rhsType = _rhs->Ty()->ToArithmType();

    if (nullptr == lhsType || nullptr == rhsType)
        Error("operand should be arithmetic type");
    if ('%' == _op && !(_lhs->Ty()->IsInteger() && _rhs->Ty()->IsInteger()))
        Error("operand of '%%' should be integer");

    //TODO: type promotion
    _ty = _lhs->Ty();
    
    return this;
}

BinaryOp* BinaryOp::AdditiveOpTypeChecking(void)
{
    //auto lhsType = _lhs->Ty()->ToArithmType();
    //auto rhsType = _rhs->Ty()->ToArithmType();


    //TODO: type promotion

    _ty = _lhs->Ty();

    return this;
}

BinaryOp* BinaryOp::ShiftOpTypeChecking(void)
{
    //TODO: type checking

    _ty = _lhs->Ty();

    return this;
}

BinaryOp* BinaryOp::RelationalOpTypeChecking(void)
{
    //TODO: type checking

    _ty = Type::NewArithmType(T_BOOL);

    return this;
}

BinaryOp* BinaryOp::EqualityOpTypeChecking(void)
{
    //TODO: type checking

    _ty = Type::NewArithmType(T_BOOL);

    return this;
}

BinaryOp* BinaryOp::BitwiseOpTypeChecking(void)
{
    if (_lhs->Ty()->IsInteger() || _rhs->Ty()->IsInteger())
        Error("operands of '&' should be integer");
    //TODO: type promotion
    _ty = Type::NewArithmType(T_INT);
    
    return this;
}

BinaryOp* BinaryOp::LogicalOpTypeChecking(void)
{
    //TODO: type checking
    if (!_lhs->Ty()->IsScalar() || !_rhs->Ty()->IsScalar())
        Error("the operand should be arithmetic type or pointer");
    _ty = Type::NewArithmType(T_BOOL);
    
    return this;
}

BinaryOp* BinaryOp::AssignOpTypeChecking(void)
{
    //TODO: type checking
    if (!_lhs->IsLVal()) {
        //TODO: error
        Error("lvalue expression expected");
    } else if (_lhs->Ty()->IsConst()) {
        Error("can't modifiy 'const' qualified expression");
    }

    _ty = _lhs->Ty();
    
    return this;
}


/************** Unary Operators *****************/

UnaryOp* UnaryOp::TypeChecking(void)
{
    switch (_op) {
    case Token::POSTFIX_INC:
    case Token::POSTFIX_DEC:
    case Token::PREFIX_INC:
    case Token::PREFIX_DEC:
        return IncDecOpTypeChecking();

    case Token::ADDR:
        return AddrOpTypeChecking();

    case Token::DEREF:
        return DerefOpTypeChecking();

    case Token::PLUS:
    case Token::MINUS:
    case '~':
    case '!':
        return UnaryArithmOpTypeChecking();

    case Token::CAST:
        return CastOpTypeChecking();

    default:
        assert(false);
        return nullptr;
    }
}

UnaryOp* UnaryOp::IncDecOpTypeChecking(void)
{
    if (!_operand->IsLVal()) {
        //TODO: error
        Error("lvalue expression expected");
    } else if (_operand->Ty()->IsConst()) {
        Error("can't modifiy 'const' qualified expression");
    }

    _ty = _operand->Ty();

    return this;
}

UnaryOp* UnaryOp::AddrOpTypeChecking(void)
{
    FuncType* funcType = _operand->Ty()->ToFuncType();
    if (nullptr != funcType && !_operand->IsLVal())
        Error("expression must be an lvalue or function designator");
    
    _ty = Type::NewPointerType(_operand->Ty());

    return this;
}

UnaryOp* UnaryOp::DerefOpTypeChecking(void)
{
    auto pointer = _operand->Ty()->ToPointerType();
    if (nullptr == pointer)
        Error("pointer expected for deref operator '*'");

    _ty = pointer->Derived();

    return this;
}

UnaryOp* UnaryOp::UnaryArithmOpTypeChecking(void)
{
    if (Token::PLUS == _op || Token::MINUS == _op) {
        if (!_operand->Ty()->IsArithm())
            Error("Arithmetic type expected");
    } else if ('~' == _op) {
        if (!_operand->Ty()->IsInteger())
            Error("integer expected for operator '~'");
    } else {//'!'
        if (!_operand->Ty()->IsScalar())
            Error("arithmetic type or pointer expected for operator '!'");
    }

    _ty = _operand->Ty();
    
    return this;
}

UnaryOp* UnaryOp::CastOpTypeChecking(void)
{
    //the _ty has been initiated to desType
    if (!_ty->IsScalar())
        Error("the cast type should be arithemetic type or pointer");
    if (_ty->IsFloat() && nullptr != _operand->Ty()->ToPointerType())
        Error("can't cast a pointer to floating");
    else if (nullptr != _ty->ToPointerType() && _operand->Ty()->IsFloat())
        Error("can't cast a floating to pointer");

    return this;
}

FuncCall* FuncCall::TypeChecking(void)
{
    auto funcType = _designator->Ty()->ToFuncType();
    if (nullptr == funcType)
        Error("not a function type");
    else
        _ty = funcType->Derived();

    //TODO: check if args and params are compatible type

    return this;
}

/*
 * Variable
 */

Variable* Variable::GetStructMember(const char* name)
{
    auto type = _ty->ToStructUnionType();
    assert(type);

    auto member = type->Find(name);
    if (member == nullptr)
        return nullptr;

    member->SetOffset(member->Offset() + _offset);

    return member;
}

Variable* Variable::GetArrayElement(TranslationUnit* unit, size_t idx)
{
    auto type = _ty->ToArrayType();
    assert(type);

    auto eleType = type->Derived();
    auto offset = _offset + eleType->Width() * idx;

    return unit->NewVariable(eleType, offset);
}

/*
 * Memory pool
 */
template <class T>
void* MemPoolImp<T>::Alloc(void)
{
    if (nullptr == _root) { //空间不够，需要分配空间
        auto block = new Block();
        _root = block->_chunks;
        //如果blocks实现为std::list, 那么push_back实际的overhead更大
        //这也表明，即使我们不需要随机访问功能(那么std::vector的拷贝是一种overhead)，
        //仍然倾向于使用std::vector，
        //当然std::vector的指数级capacity增长会造成内存浪费。
        _blocks.push_back(block);
    }
    
    auto ret = _root;
    _root = _root->_next;

    return ret;
}

template <class T>
void MemPoolImp<T>::Free(void* addr)
{
    if (nullptr == addr) 
        return;

    auto chunk = static_cast<Chunk*>(addr);
    chunk->_next = _root;
    _root = chunk;
}

template <class T>
void MemPoolImp<T>::Clear(void)
{
    for (auto iter = _blocks.begin(); iter != _blocks.end(); iter++)
        delete *iter;
        
    _root = nullptr;
}

/*
 * Translation unit
 */
ConditionalOp* TranslationUnit::NewConditionalOp(Expr* cond,
        Expr* exprTrue, Expr* exprFalse)
{
    auto ret = new (_conditionalOpPool.Alloc())
            ConditionalOp(&_conditionalOpPool, cond, exprTrue, exprFalse);

    ret->TypeChecking();
    return ret;
}

BinaryOp* TranslationUnit::NewBinaryOp(int op, Expr* lhs, Expr* rhs)
{
    switch (op) {
    case '[':
    case '*':
    case '/':
    case '%':
    case '+':
    case '-':
    case '&':
    case '^':
    case '|':
    case '<':
    case '>':
    case Token::LEFT_OP:
    case Token::RIGHT_OP:
    case Token::LE_OP:
    case Token::GE_OP:
    case Token::EQ_OP:
    case Token::NE_OP: 
    case Token::AND_OP:
    case Token::OR_OP:
        break;

    default:
        assert(0);
    }

    auto ret = new (_binaryOpPool.Alloc()) BinaryOp(&_binaryOpPool, op, lhs, rhs);
    ret->TypeChecking();
    
    return ret;
}

BinaryOp* TranslationUnit::NewMemberRefOp(int op, Expr* lhs, const char* rhsName)
{
    assert('.' == op || Token::PTR_OP == op);
    
    //the initiation of rhs is lefted in type checking
    auto ret = new (_binaryOpPool.Alloc())
            BinaryOp(&_binaryOpPool, op, lhs, nullptr);
    
    ret->MemberRefOpTypeChecking(rhsName);

    return ret;
}

/*
UnaryOp* TranslationUnit::NewUnaryOp(Type* type, int op, Expr* expr) {
    return new UnaryOp(type, op, expr);
}
*/


FuncCall* TranslationUnit::NewFuncCall(Expr* designator, const std::list<Expr*>& args)
{
    auto ret = new (_funcCallPool.Alloc()) FuncCall(&_funcCallPool, designator, args);
    ret->TypeChecking();
    
    return ret;
}

Variable* TranslationUnit::NewVariable(Type* type, int offset)
{
    auto ret = new (_variablePool.Alloc()) Variable(&_variablePool, type, offset);

    return ret;
}

Constant* TranslationUnit::NewConstantInteger(ArithmType* type, long long val)
{
    auto ret = new (_constantPool.Alloc()) Constant(&_constantPool, type, val);

    return ret;
}

Constant* TranslationUnit::NewConstantFloat(ArithmType* type, double val)
{
    auto ret = new (_constantPool.Alloc()) Constant(&_constantPool, type, val);

    return ret;
}


TempVar* TranslationUnit::NewTempVar(Type* type)
{
    auto ret = new (_tempVarPool.Alloc()) TempVar(&_tempVarPool, type);

    return ret;
}

UnaryOp* TranslationUnit::NewUnaryOp(int op, Expr* operand, Type* type)
{
    auto ret = new (_unaryOpPool.Alloc()) UnaryOp(&_unaryOpPool, op, operand, type);
    ret->TypeChecking();

    return ret;
}


/********** Statement ***********/

//��Ȼ��stmtֻ��Ҫһ��
EmptyStmt* TranslationUnit::NewEmptyStmt(void)
{
    auto ret = new (_emptyStmtPool.Alloc()) EmptyStmt(&_emptyStmtPool);

    return ret;
}

//else stmt Ĭ���� null
IfStmt* TranslationUnit::NewIfStmt(Expr* cond, Stmt* then, Stmt* els)
{
    auto ret = new (_ifStmtPool.Alloc()) IfStmt(&_ifStmtPool, cond, then, els);

    return ret;
}

CompoundStmt* TranslationUnit::NewCompoundStmt(std::list<Stmt*>& stmts)
{
    auto ret = new (_compoundStmtPool.Alloc())
            CompoundStmt(&_compoundStmtPool, stmts);

    return ret;
}

JumpStmt* TranslationUnit::NewJumpStmt(LabelStmt* label)
{
    auto ret = new (_jumpStmtPool.Alloc()) JumpStmt(&_jumpStmtPool, label);

    return ret;
}

ReturnStmt* TranslationUnit::NewReturnStmt(Expr* expr)
{
    auto ret = new (_returnStmtPool.Alloc())
            ReturnStmt(&_returnStmtPool, expr);

    return ret;
}

LabelStmt* TranslationUnit::NewLabelStmt(void)
{
    auto ret = new (_labelStmtPool.Alloc()) LabelStmt(&_labelStmtPool);

    return ret;
}

FuncDef* TranslationUnit::NewFuncDef(FuncType* type, CompoundStmt* stmt)
{
    auto ret = new (_funcDefPool.Alloc()) FuncDef(&_funcDefPool, type, stmt);
    
    return ret;
}

void TranslationUnit::Delete(ASTNode* node)
{
    if (node == nullptr)
        return;

    MemPool* pool = node->_pool;
    node->~ASTNode();
    pool->Free(node);
}