class Blob {
  ArrayList<PVector> points;
  PVector centroid;
  PVector sum;

  Blob(PVector first_point) {
    points = new ArrayList<PVector>();
    centroid = new PVector(first_point.x, first_point.y);
    sum = new PVector(first_point.x, first_point.y);
    points.add(first_point);
  }

  boolean isNear(PVector in, int thresh) {
    float dist = centroid.dist(in);
    //println("dist ",dist);
    if (dist < thresh) {
      points.add(in);
      sum.add(in);
      calcCentroid();
      //println("centroid",centroid);
      return true;
    }

    return false;
  }

  void calcCentroid() {
    //println("sum",sum);
    centroid = sum.copy().div(points.size());
  }
  
  float calcRadius() {
    float max_dist = -1;
    for(PVector pt : points){
      float dist = centroid.dist(pt);
      if(dist > max_dist) max_dist = dist;
    };
    return max_dist;
  }

  void display(float ratio, float size, String name, PGraphics img) {
    //for (PVector pt : points) {
    //  point(pt.x * ratio, pt.y * ratio);
    //}

    img.stroke(255, 0, 0);
    img.noFill();

    img.ellipse(centroid.x * ratio, centroid.y * ratio, size, size);
    img.fill(255, 0, 0);
    img.textSize(60);
    img.text(name, centroid.x * ratio, centroid.y * ratio);
  }
}
