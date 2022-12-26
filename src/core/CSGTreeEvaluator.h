#pragma once

#include <map>
#include <list>
#include <vector>
#include <cstddef>
#include "NodeVisitor.h"
#include "memory.h"
#include "CSGNode.h"

class CSGNode;
class GeometryEvaluator;
class Tree;

class CSGTreeEvaluator : public NodeVisitor
{
public:
  CSGTreeEvaluator(const Tree& tree, GeometryEvaluator *geomevaluator = nullptr)
    : tree(tree), geomevaluator(geomevaluator) {
  }

  Response visit(State& state, const AbstractNode& node) override;
  Response visit(State& state, const AbstractIntersectionNode& node) override;
  Response visit(State& state, const AbstractPolyNode& node) override;
  Response visit(State& state, const ListNode& node) override;
  Response visit(State& state, const CsgOpNode& node) override;
  Response visit(State& state, const TransformNode& node) override;
  Response visit(State& state, const ColorNode& node) override;
  Response visit(State& state, const RenderNode& node) override;
  Response visit(State& state, const CgalAdvNode& node) override;

  shared_ptr<CSGNode> buildCSGTree(const AbstractNode& node);

  [[nodiscard]] const shared_ptr<CSGNode>& getRootNode() const {
    return this->rootNode;
  }
  [[nodiscard]] const std::vector<shared_ptr<CSGNode>>& getHighlightNodes() const {
    return this->highlightNodes;
  }
  [[nodiscard]] const std::vector<shared_ptr<CSGNode>>& getBackgroundNodes() const {
    return this->backgroundNodes;
  }

private:
  void addToParent(const State& state, const AbstractNode& node);
  void applyToChildren(State& state, const AbstractNode& node, OpenSCADOperator op);
  shared_ptr<CSGNode> evaluateCSGNodeFromGeometry(State& state,
                                                  const shared_ptr<const Geometry>& geom,
                                                  const ModuleInstantiation *modinst,
                                                  const AbstractNode& node);
  void applyBackgroundAndHighlight(State& state, const AbstractNode& node);

  using ChildList = std::list<std::shared_ptr<const AbstractNode>>;
  std::map<int, ChildList> visitedchildren;

protected:
  const Tree& tree;
  GeometryEvaluator *geomevaluator;
  shared_ptr<CSGNode> rootNode;
  std::vector<shared_ptr<CSGNode>> highlightNodes;
  std::vector<shared_ptr<CSGNode>> backgroundNodes;
  std::map<int, shared_ptr<CSGNode>> stored_term; // The term evaluated from each node index
};
