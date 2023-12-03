optional<Edge*> HalfedgeMesh::flip_edge(Edge* e)
{
    if (!e || e->on_boundary()) {
        return std::nullopt; // 不能翻转边界上的边
    }

    Halfedge* h_1_2 = e->halfedge;
    Halfedge* h_2_1 = h_1_2->inv;
    Halfedge* h_2_3 = h_1_2->next;
    Halfedge* h_3_1 = h_2_3->next;
    Halfedge* h_1_4 = h_2_1->next;
    Halfedge* h_4_2 = h_1_4->next;

    Vertex* v1 = h_1_2->from;
    Vertex* v2 = h_2_1->from;
    Vertex* v3 = h_3_1->from;
    Vertex* v4 = h_4_2->from;

    Halfedge* h_4_3 = h_1_2;
    Halfedge* h_3_4 = h_2_1;

    Face* f1 = h_4_3->face;
    Face* f2 = h_3_4->face;

    h_4_3->next = h_3_1;
    h_4_3->prev = h_1_4;
    h_4_3->from = v4;
    h_4_3->face = f1;

    h_3_4->next = h_4_2;
    h_3_4->prev = h_2_3;
    h_3_4->from = v3;
    h_3_4->face = f2;

    h_3_1->prev = h_4_3;
    h_3_1->next = h_1_4;
    h_3_1->face = f1;
    
    h_1_4->prev = h_3_1;
    h_1_4->next = h_4_3;
    h_1_4->face = f1;

    h_4_2->prev = h_3_4;
    h_4_2->next = h_2_3;
    h_4_2->face = f2;

    h_2_3->prev = h_4_2;
    h_2_3->next = h_3_4;
    h_4_2->face = f2;

    v1->halfedge = h_1_4;
    v2->halfedge = h_2_3;
    v3->halfedge = h_3_1;
    v3->halfedge = h_4_2;

    f1->halfedge = h_4_3;
    f2->halfedge = h_3_4;

    return e;
}

optional<Vertex*> HalfedgeMesh::split_edge(Edge* e) {
    if (!e) {
        spdlog::info("[split_edge] Error: Edge is null.");
        return std::nullopt;
    }

    Halfedge* h_1_2 = e->halfedge;
    Halfedge* h_2_1 = h_1_2->inv;
    Halfedge* h_2_3 = h_1_2->next;
    Halfedge* h_3_1 = h_2_3->next;
    Halfedge* h_1_4 = h_2_1->next;
    Halfedge* h_4_2 = h_1_4->next;

    Vertex* v1 = h_1_2->from;
    Vertex* v2 = h_2_1->from;
    Vertex* v3 = h_3_1->from;
    Vertex* v4 = h_4_2->from;

    Halfedge* h_1_n = h_1_2;
    Halfedge* h_2_n = h_2_1;

    Face* f1_n_3 = h_1_n->face;
    Face* f2_n_4 = h_2_n->face;
    Face* fn_2_3 = new_face();
    Face* fn_1_4 = new_face();

    Halfedge* h_n_3 = new_halfedge();
    Halfedge* h_3_n = new_halfedge();
    Halfedge* h_n_2 = new_halfedge();
    Halfedge* h_n_1 = new_halfedge();
    Halfedge* h_n_4 = new_halfedge();
    Halfedge* h_4_n = new_halfedge();
    
    Edge* e1 = e;
    Edge* e2 = new_edge();
    Edge* e3 = new_edge();
    Edge* e4 = new_edge();

    Vertex* vnew = new_vertex();

    h_3_1->set_neighbors(h_1_n,h_n_3,h_3_1->inv,v3,h_3_1->edge,f1_n_3);
    h_2_3->set_neighbors(h_3_n,h_n_2,h_2_3->inv,v2,h_2_3->edge,fn_2_3);
    h_1_4->set_neighbors(h_4_n,h_n_1,h_1_4->inv,v1,h_1_4->edge,fn_1_4);
    h_4_2->set_neighbors(h_2_n,h_n_4,h_4_2->inv,v4,h_4_2->edge,f2_n_4);

    h_1_n->set_neighbors(h_n_3,h_3_1,h_n_1,v1,e1,f1_n_3);
    h_n_1->set_neighbors(h_1_4,h_4_n,h_1_n,vnew,e1,fn_1_4);

    h_n_3->set_neighbors(h_3_1,h_1_n,h_3_n,vnew,e2,f1_n_3);
    h_3_n->set_neighbors(h_n_2,h_2_3,h_n_3,v3,e2,fn_2_3);

    h_n_2->set_neighbors(h_2_3,h_3_n,h_2_n,vnew,e3,fn_2_3);
    h_2_n->set_neighbors(h_n_4,h_4_2,h_n_2,v2,e3,f2_n_4);

    h_4_n->set_neighbors(h_n_1,h_1_4,h_n_4,v4,e4,fn_1_4);
    h_n_4->set_neighbors(h_4_2,h_2_n,h_4_n,vnew,e4,f2_n_4);

    v1->halfedge = h_1_n;
    v2->halfedge = h_2_n;
    v3->halfedge = h_3_n;
    v4->halfedge = h_4_n;
    vnew->halfedge = h_n_3;
    vnew->pos = vnew->neighborhood_center();
    vnew->is_new = true;

    f1_n_3->halfedge = h_1_n;
    fn_2_3->halfedge = h_3_n;
    fn_1_4->halfedge = h_4_n;
    f2_n_4->halfedge = h_n_4;

    e1->halfedge = h_1_n;
    e2->halfedge = h_3_n;
    e3->halfedge = h_2_n;
    e4->halfedge = h_4_n;

    //cout<<h_3_1->from<<" "<<h_1_n->from<<" "<<h_n_3->from<<endl;
    //cout<<h_3_n->from<<" "<<h_n_2->from<<" "<<h_2_3->from<<endl;
    //cout<<h_n_1->from<<" "<<h_1_4->from<<" "<<h_4_n->from<<endl;
    //cout<<h_n_4->from<<" "<<h_4_2->from<<" "<<h_2_n->from<<endl;

    cout<<vnew->id<<endl;

    return vnew;
}

optional<Vertex*> HalfedgeMesh::collapse_edge(Edge* e)
{
    if (!e) {
        return std::nullopt;
    }

    Halfedge* h_1_2 = e->halfedge;
    Halfedge* h_2_1 = h_1_2->inv;
    Halfedge* h_2_3 = h_1_2->next;
    Halfedge* h_3_1 = h_2_3->next;
    Halfedge* h_1_4 = h_2_1->next;
    Halfedge* h_4_2 = h_1_4->next;
    
    Halfedge* h_1_3 = h_3_1->inv;
    Halfedge* h_3_2 = h_2_3->inv;
    Halfedge* h_4_1 = h_1_4->inv;
    Halfedge* h_2_4 = h_4_2->inv;

    Vertex* v1 = h_1_2->from;
    Vertex* v2 = h_2_1->from;
    Vertex* v3 = h_3_1->from;
    Vertex* v4 = h_4_2->from;

    Face* f1 = h_1_2->face;
    Face* f2 = h_2_1->face;

    Edge* e1_3 = h_1_3->edge;
    Edge* e3_2 = h_3_2->edge;
    Edge* e2_4 = h_2_4->edge;
    Edge* e4_1 = h_4_1->edge;

    Halfedge* h = v2->halfedge;
    do {
        //Vertex* neighbor = h->inv->from;
        h->from = v1;
        h = h->inv->next;
        //cout<<1<<endl;
    } while (h != v2->halfedge);

    h_1_3->inv = h_3_2;
    h_3_2->inv = h_1_3;
    h_4_1->inv = h_2_4;
    h_2_4->inv = h_4_1;

    h_3_2->edge = e1_3;
    h_4_1->edge = e2_4;

    v1->halfedge = h_1_3;
    v3->halfedge = h_3_2;
    v4->halfedge = h_4_1;

    v1->pos = v1->neighborhood_center();
    v3->pos = v3->neighborhood_center();
    v4->pos = v4->neighborhood_center();
    
    erase(h_3_1);
    erase(h_1_2);
    erase(h_2_3);
    erase(h_2_1);
    erase(h_1_4);
    erase(h_4_2);

    erase(f1);
    erase(f2);
    
    erase(e);
    erase(e3_2);
    erase(e4_1);
    
    erase(v2);

    return v1;
}
